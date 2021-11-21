//
// Created by zhaokai on 2020/7/14.
//

#ifndef HUMANVIDEOEFFECTS_GLCONTEXT_H
#define HUMANVIDEOEFFECTS_GLCONTEXT_H

#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "gl_base.h"
#include <base/Status.h>
#include <future>


namespace cutils {

    typedef std::function<void()> GlVoidFunction;
    typedef std::function<Status()> GlStatusFunction;

    class GlContext;

// Generic interface for synchronizing access to a shared resource from a
// different context. This is an abstract class to keep users from
// depending on its contents. The implementation may differ depending on
// the capabilities of the GL context.
    class GlSyncPoint {
    public:
        explicit GlSyncPoint(const std::shared_ptr <GlContext> &gl_context)
                : gl_context_(gl_context) {}

        virtual ~GlSyncPoint() {}

        // Waits until the GPU has executed all commands up to the sync point.
        // This blocks the CPU, and ensures the commands are complete from the
        // point of view of all threads and contexts.
        virtual void Wait() = 0;

        // Ensures that the following commands on the current OpenGL context will
        // not be executed until the sync point has been reached.
        // This does not block the CPU, and only affects the current OpenGL context.
        virtual void WaitOnGpu() { Wait(); }

        // Returns whether the sync point has been reached. Does not block.
        virtual bool IsReady() = 0;

        const GlContext &GetContext() { return *gl_context_; }

    protected:
        std::shared_ptr <GlContext> gl_context_;
    };

// Combines sync points for multiple contexts.
    class GlMultiSyncPoint : public GlSyncPoint {
    public:
        GlMultiSyncPoint() : GlSyncPoint(nullptr) {}

        // Adds a new sync to the multisync.
        // If we already have a sync from the same context, overwrite it.
        // Commands on the same context are serialized, and we only care about
        // when the last one is done.
        void Add(std::shared_ptr<GlSyncPoint> new_sync);

        void Wait() override;

        void WaitOnGpu() override;

        bool IsReady() override;

    private:
        std::vector <std::shared_ptr<GlSyncPoint>> syncs_;
    };

// TODO: remove.
    typedef std::shared_ptr <GlSyncPoint> GlSyncToken;

    typedef EGLContext PlatformGlContext;
    constexpr PlatformGlContext kPlatformGlContextNone = EGL_NO_CONTEXT;

// This class provides a common API for creating and managing GL contexts.
//
// It handles the following responsibilities:
// - Providing a cross-platform interface over platform-specific APIs like EGL
//   and EAGL.
// - Managing the interaction between threads and GL contexts.
// - Managing synchronization between different GL contexts.
//
    class GlContext : public std::enable_shared_from_this<GlContext> {
    public:
        using GlContextPtr = std::shared_ptr<GlContext>;

        // Creates a GlContext.
        //
        // The first argument (which can be a GlContext, or a platform-specific type)
        // indicates a context with which to share resources (e.g. textures).
        // Resources will be shared amongst all contexts linked in this way. You can
        // pass null if sharing is not desired.
        //
        // If create_thread is true, the context will create a thread and run all
        // OpenGL tasks on it.
        static GlContextPtr Create(std::nullptr_t nullp, bool create_thread);

        static GlContextPtr Create(const GlContext &share_context,
                                   bool create_thread);

        static GlContextPtr Create(PlatformGlContext share_context,
                                   bool create_thread);

        // Returns the GlContext that is current on this thread. May return nullptr.
        static std::shared_ptr <GlContext> GetCurrent();

        GlContext(const GlContext &) = delete;

        GlContext &operator=(const GlContext &) = delete;

        ~GlContext();

        // Executes a function in the GL context. Waits for the
        // function's execution to be complete before returning to the caller.
        Status Run(GlStatusFunction gl_func, int node_id = -1,
                   Timestamp input_timestamp = std::numeric_limits<uint64_t>::max());

        // Like Run, but does not wait.
        void RunWithoutWaiting(GlVoidFunction gl_func);
        template <typename T>
        std::future<T> Post(std::function<T()> gl_func);

        // Returns a synchronization token.
        // This should not be called outside of the GlContext thread.
        std::shared_ptr <GlSyncPoint> CreateSyncToken();

        // If another part of the framework calls glFinish, it should call this
        // method to let the context know that it has done so. The context can use
        // that information to avoid inserting additional glFinish calls in some
        // cases.
        void GlFinishCalled();

        // Ensures that the changes to shared resources covered by the token are
        // visible in the current context.
        // This should only be called outside a job.
        void WaitSyncToken(const std::shared_ptr <GlSyncPoint> &token);

        // Checks whether the token's sync point has been reached. Returns true
        // iff WaitSyncToken would not have to wait.
        // This is thread-safe.
        bool SyncTokenIsReady(const std::shared_ptr <GlSyncPoint> &token);

        // Returns the EGLDisplay used by our context.
        EGLDisplay egl_display() const { return display_; }

        // Returns the EGLConfig used to create our context.
        EGLConfig egl_config() const { return config_; }

        // Returns our EGLContext.
        EGLContext egl_context() const { return context_; }

        // Check if the context is current on this thread. Mainly for test purposes.
        bool IsCurrent() const;

        GLint gl_major_version() const { return gl_major_version_; }

        GLint gl_minor_version() const { return gl_minor_version_; }

        static bool ParseGlVersion(std::string version, GLint *major,
                                   GLint *minor);

        int64_t gl_finish_count() { return gl_finish_count_; }

        // Used by GlFinishSyncPoint. The count_to_pass cannot exceed the current
        // gl_finish_count_ (but it can be equal).
        void WaitForGlFinishCountPast(int64_t count_to_pass);

        // Convenience version of Run for arguments with a void result type.
        // Waits for the function to finish executing before returning.
        //
        // Implementation note: we cannot use a std::function<void(void)> argument
        // here, because that would break passing in a lambda that returns a status;
        // e.g.:
        //   RunInGlContext([]() -> Status { ... });
        //
        // The reason is that std::function<void(...)> allows the implicit conversion
        // of a callable with any result type, as long as the argument types match.
        // As a result, the above lambda would be implicitly convertible to both
        // std::function<Status(void)> and std::function<void(void)>, and
        // the invocation would be ambiguous.
        //
        // Therefore, instead of using std::function<void(void)>, we use a template
        // that only accepts arguments with a void result type.
        template<typename T, typename = typename std::enable_if<std::is_void<
                typename std::result_of<T()>::type>::value>::type>
        void Run(T f) {
            Run([f] {
                f();
                return OkStatus();
            }).IgnoreError();
        }

        // These are used for testing specific SyncToken implementations. Do not use
        // outside of tests.
        enum class SyncTokenTypeForTest {
            kGlFinish,
        };

        std::shared_ptr <GlSyncPoint> TestOnly_CreateSpecificSyncToken(
                SyncTokenTypeForTest type);

    private:
        GlContext();

        Status CreateContext(EGLContext share_context);

        Status CreateContextInternal(EGLContext share_context,
                                     int gl_version);

        EGLDisplay display_ = EGL_NO_DISPLAY;
        EGLConfig config_;
        EGLSurface surface_ = EGL_NO_SURFACE;
        EGLContext context_ = EGL_NO_CONTEXT;

        class DedicatedThread;

        // A context binding represents the minimal set of information needed to make
        // a context current on a thread. Its contents depend on the platform.
        struct ContextBinding {
            // The context_object is null if this binding refers to a context not
            // managed by GlContext.
            std::weak_ptr <GlContext> context_object;
            EGLDisplay display = EGL_NO_DISPLAY;
            EGLSurface draw_surface = EGL_NO_SURFACE;
            EGLSurface read_surface = EGL_NO_SURFACE;
            EGLContext context = EGL_NO_CONTEXT;
        };

        Status FinishInitialization(bool create_thread);

        // This wraps a thread_local.
        static std::weak_ptr <GlContext> &CurrentContext();

        static Status SwitchContext(ContextBinding *old_context,
                                    const ContextBinding &new_context);

        Status EnterContext(ContextBinding *previous_context);

        Status ExitContext(const ContextBinding *previous_context);

        void DestroyContext();

        bool HasContext() const;

        bool CheckForGlErrors();

        void LogUncheckedGlErrors(bool had_gl_errors);

        // The following ContextBinding functions have platform-specific
        // implementations.

        // A binding that can be used to make this GlContext current.
        ContextBinding ThisContextBinding();

        // Fills in a ContextBinding with platform-specific information about which
        // context is current on this thread.
        static void GetCurrentContextBinding(ContextBinding *binding);

        // Makes the context described by new_context current on this thread.
        static Status SetCurrentContextBinding(
                const ContextBinding &new_context);

        // If not null, a dedicated thread used to execute tasks on this context.
        // Used on Android due to expensive context switching on some configurations.
        std::unique_ptr<DedicatedThread> thread_;

        GLint gl_major_version_ = 0;
        GLint gl_minor_version_ = 0;

        // Number of glFinish calls completed on the GL thread.
        // Changes should be guarded by mutex_. However, we use simple atomic
        // loads for efficiency on the fast path.
        std::atomic <int64_t> gl_finish_count_ = ATOMIC_VAR_INIT(0);

        // This mutex is held by a thread while this GL context is current on that
        // thread. Since it may be held for extended periods of time, it should not
        // be used for other pieces of status.
        std::mutex context_use_mutex_;

        // This mutex is used to guard a few different members and condition
        // variables. It should only be held for a short time.
        std::mutex mutex_;
        std::condition_variable wait_for_gl_finish_cv_;


    };



}


#endif //HUMANVIDEOEFFECTS_GLCONTEXT_H
