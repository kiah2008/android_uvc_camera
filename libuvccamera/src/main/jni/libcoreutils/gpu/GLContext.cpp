//
// Created by zhaokai on 2020/7/14.
//

#include <pthread.h>
#include <ios>

#include <gpu/GLContext_internal.h>
#include <port/clog.h>
#include <utils/str_helper.h>
#include <gpu/gl_base.h>
#include <gpu/GLContext.h>
#include <utils/ScopedTimer.h>

using namespace cutils;


static pthread_key_t egl_release_thread_key;
static pthread_once_t egl_release_key_once = PTHREAD_ONCE_INIT;

static void EglThreadExitCallback(void *key_value) {

    if(EGL_NO_DISPLAY != eglGetCurrentDisplay()) {
        CLOGD("release current display");
        eglMakeCurrent(EGL_NO_DISPLAY, EGL_NO_SURFACE, EGL_NO_SURFACE,
                   EGL_NO_CONTEXT);
    }
    eglReleaseThread();
}

// If a key has a destructor callback, and a thread has a non-NULL value for
// that key, then the destructor is called when the thread exits.
static void MakeEglReleaseThreadKey() {
    int err = pthread_key_create(&egl_release_thread_key, EglThreadExitCallback);
    if (err) {
        CLOGE("cannot create pthread key: %d", err);
    }
}

// This function can be called any number of times. For any thread on which it
// was called at least once, the EglThreadExitCallback will be called (once)
// when the thread exits.
static void EnsureEglThreadRelease() {
    pthread_once(&egl_release_key_once, MakeEglReleaseThreadKey);
    pthread_setspecific(egl_release_thread_key,
                        reinterpret_cast<void *>(0xDEADBEEF));
}

GlContext::GlContextPtr GlContext::Create(std::nullptr_t nullp,
                                          bool create_thread) {
    return Create(EGL_NO_CONTEXT, create_thread);
}

GlContext::GlContextPtr GlContext::Create(const GlContext &share_context,
                                          bool create_thread) {
    return Create(share_context.context_, create_thread);
}

GlContext::GlContextPtr GlContext::Create(EGLContext share_context,
                                          bool create_thread) {
    std::shared_ptr<GlContext> context(new GlContext());
    CUTILS_CHECK_RETURN_STATUS(context->CreateContext(share_context).ok(), nullptr, "CreateContext failed!");
    CUTILS_CHECK_RETURN_STATUS(context->FinishInitialization(create_thread).ok(), nullptr,
                                "Create failed");
    return std::move(context);
}

Status GlContext::CreateContextInternal(
        EGLContext external_context, int gl_version) {
    CUTILS_CHECK(gl_version == 2 || gl_version == 3);

    const EGLint config_attr[] = {
            // clang-format off
            EGL_RENDERABLE_TYPE, gl_version == 3 ? EGL_OPENGL_ES3_BIT_KHR
                                                 : EGL_OPENGL_ES2_BIT,
            // Allow rendering to pixel buffers or directly to windows.
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT | EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,  // if you need the alpha channel
//            EGL_DEPTH_SIZE, 8,  // if you need the depth buffer
//            EGL_STENCIL_SIZE, EGL_DONT_CARE,
            EGL_NONE
            // clang-format on
    };

    // TODO: improve config selection.
    EGLint num_configs;
    EGLBoolean success =
            eglChooseConfig(display_, config_attr, &config_, 1, &num_configs);
    if (!success) {
        Status status(CUTILS_STATUS_UNKNOWN);
        status << "eglChooseConfig() returned error " << eglGetError();
        return status;
    }
    if (!num_configs) {
        Status status(CUTILS_STATUS_UNKNOWN);
        status << "eglChooseConfig() returned no matching EGL configuration for "
               << "RGBA8888 D16 ES" << gl_version << " request. ";
        return status;
    }

    const EGLint context_attr[] = {
            // clang-format off
            EGL_CONTEXT_CLIENT_VERSION, gl_version,
            EGL_NONE
            // clang-format on
    };

    context_ =
            eglCreateContext(display_, config_, external_context, context_attr);
    int error = eglGetError();
    CUTILS_RETURN_STATUS_IF_ERROR(context_ != EGL_NO_CONTEXT, Status(CUTILS_STATUS_UNKNOWN, StrCat(
            "Could not create GLES %d context, return error:%x, %s", gl_version, error,
            error == EGL_BAD_CONTEXT
            ? ": external context uses a different version of OpenGL"
            : "")));

    // We can't always rely on GL_MAJOR_VERSION and GL_MINOR_VERSION, since
    // GLES 2 does not have them, so let's set the major version here at least.
    gl_major_version_ = gl_version;

    return OkStatus();
}

Status GlContext::CreateContext(EGLContext external_context) {
    EGLint major = 0;
    EGLint minor = 0;

    display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    CUTILS_RETURN_STATUS_IF_ERROR(display_ != EGL_NO_DISPLAY, Status(CUTILS_STATUS_UNKNOWN,
                                                                  StrCat("eglGetDisplay() returned error ",
                                                                         eglGetError())));

    EGLBoolean success = eglInitialize(display_, &major, &minor);
    if (!success) {
        return Status(CUTILS_STATUS_UNKNOWN, "unable initialize egl");
    }
    CLOG("Successfully initialized EGL. Major : %d, minor: %d", major, minor);

    auto status = CreateContextInternal(external_context, 3);
    if (!status.ok()) {
        CLOGW("Creating a context with OpenGL ES 3 failed: %d", status.code());
        CLOGW("Fall back on OpenGL ES 2.");
        status = CreateContextInternal(external_context, 2);
    }
    CUTILS_RETURN_STATUS_IF_ERROR(status.ok(), status);

    EGLint pbuffer_attr[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};

    surface_ = eglCreatePbufferSurface(display_, config_, pbuffer_attr);
    CUTILS_RETURN_STATUS_IF_ERROR(surface_ != EGL_NO_SURFACE, Status(CUTILS_STATUS_UNKNOWN,
                                                                  StrCat("eglCreatePbufferSurface() returned error ",
                                                                         eglGetError())));

    return OkStatus();
}

void GlContext::DestroyContext() {
    ScopedTimer _t("DestroyContext");
    if (thread_) {
        // Delete thread-local storage.
        // TODO: in theory our EglThreadExitCallback should suffice for
        // this; however, heapcheck still reports a leak without this call here
        // when using SwiftShader.
        // Perhaps heapcheck misses the thread destructors?
        thread_->Run([] {
                    eglReleaseThread();
                    return OkStatus();
                })
                .IgnoreError();
    }

    // Destroy the context and surface.
    if (IsCurrent()) {
        if (!eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE,
                            EGL_NO_CONTEXT)) {
            CLOGE("eglMakeCurrent() returned error %d", eglGetError());
        }
    }
    if (surface_ != EGL_NO_SURFACE) {
        if (!eglDestroySurface(display_, surface_)) {
            CLOGE("eglDestroySurface() returned error %d", eglGetError());
        }
        surface_ = EGL_NO_SURFACE;
    }
    if (context_ != EGL_NO_CONTEXT) {
        if (!eglDestroyContext(display_, context_)) {
            CLOGE("eglDestroyContext() returned error %d", eglGetError());
        }
        context_ = EGL_NO_CONTEXT;
    }

    // Under standard EGL, eglTerminate will terminate the display connection
    // for the entire process, no matter how many times eglInitialize has been
    // called. So we do not want to terminate it here, in case someone else is
    // using it.
    // However, Android implements non-standard reference-counted semantics for
    // eglInitialize/eglTerminate, so we should call it on that platform.
#ifdef __ANDROID__
    // TODO: this is removed for now since it caused issues on
    // YouTube. But in theory we _should_ be calling it. Needs more
    // investigation.
    // eglTerminate(display_);
#endif  // __ANDROID__
}

GlContext::ContextBinding GlContext::ThisContextBinding() {
    GlContext::ContextBinding result;
    result.context_object = shared_from_this();
    result.display = display_;
    result.draw_surface = surface_;
    result.read_surface = surface_;
    result.context = context_;
    return result;
}

void GlContext::GetCurrentContextBinding(GlContext::ContextBinding *binding) {
    binding->display = eglGetCurrentDisplay();
    binding->draw_surface = eglGetCurrentSurface(EGL_DRAW);
    binding->read_surface = eglGetCurrentSurface(EGL_READ);
    binding->context = eglGetCurrentContext();
}

Status GlContext::SetCurrentContextBinding(
        const ContextBinding &new_binding) {
    EnsureEglThreadRelease();
    EGLDisplay display = new_binding.display;
    if (display == EGL_NO_DISPLAY) {
        display = eglGetCurrentDisplay();
    }
    if (display == EGL_NO_DISPLAY) {
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    }
    EGLBoolean success =
            eglMakeCurrent(display, new_binding.draw_surface,
                           new_binding.read_surface, new_binding.context);
    if (!success) {
        return Status(CUTILS_STATUS_UNKNOWN,
                      StrCat("eglMakeCurrent() returned error!", eglGetError()).c_str());
    }
    return OkStatus();
}

bool GlContext::HasContext() const { return context_ != EGL_NO_CONTEXT; }

bool GlContext::IsCurrent() const {
    return HasContext() && (eglGetCurrentContext() == context_);
}


static void SetThreadName(const char *name) {
#define LINUX_STYLE_SETNAME_NP 1
    char thread_name[16];  // Linux requires names (with nul) fit in 16 chars
    strncpy(thread_name, name, sizeof(thread_name));
    thread_name[sizeof(thread_name) - 1] = '\0';
    int res = pthread_setname_np(pthread_self(), thread_name);
    if (res != 0) {
        CLOGD("Can't set pthread names: name: \"%s\", error:%d", name, res);
    }
}

GlContext::DedicatedThread::DedicatedThread() {
    CUTILS_CHECK_EQ(pthread_create(&gl_thread_id_, nullptr, ThreadBody, this), 0);
}

GlContext::DedicatedThread::~DedicatedThread() {
    if (IsCurrentThread()) {
        CUTILS_CHECK(self_destruct_);
        CUTILS_CHECK_EQ(pthread_detach(gl_thread_id_), 0);
    } else {
        // Give an invalid job to signal termination.
        PutJob({});
        CUTILS_CHECK_EQ(pthread_join(gl_thread_id_, nullptr), 0);
    }
}

void GlContext::DedicatedThread::SelfDestruct() {
    self_destruct_ = true;
    // Give an invalid job to signal termination.
    PutJob({});
}

GlContext::DedicatedThread::Job GlContext::DedicatedThread::GetJob() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (jobs_.empty()) {
        has_jobs_cv_.wait(lock);
    }
    Job job = std::move(jobs_.front());
    jobs_.pop_front();
    return job;
}

void GlContext::DedicatedThread::PutJob(Job job) {
    std::lock_guard<std::mutex> lock(mutex_);
    jobs_.push_back(std::move(job));
    has_jobs_cv_.notify_all();
}

void *GlContext::DedicatedThread::ThreadBody(void *instance) {
    DedicatedThread *thread = static_cast<DedicatedThread *>(instance);
    thread->ThreadBody();
    return nullptr;
}

#ifdef __APPLE__
#define AUTORELEASEPOOL @autoreleasepool
#else
#define AUTORELEASEPOOL
#endif  // __APPLE__

void GlContext::DedicatedThread::ThreadBody() {
    SetThreadName("fastpipe_gl_runner");
    // The dedicated GL thread is not meant to be used on Apple platforms, but
    // in case it is, the use of an autorelease pool here will reap each task's
    // temporary allocations.
    while (true) AUTORELEASEPOOL {
        Job job = GetJob();
        // Lack of a job means termination. Or vice versa.
        if (!job) {
            break;
        }
        job();
    }
    if (self_destruct_) {
        delete this;
    }
}

Status GlContext::DedicatedThread::Run(GlStatusFunction gl_func) {
    // Neither ENDO_SCOPE nor ENDO_TASK seem to work here.
    if (IsCurrentThread()) {
        return gl_func();
    }
    bool done = false;  // Guarded by mutex_ after initialization.
    Status status;
    PutJob([this, gl_func, &done, &status]() {
        status = gl_func();
        std::lock_guard<std::mutex> lock(mutex_);
        done = true;
        gl_job_done_cv_.notify_all();
    });

    std::unique_lock<std::mutex> lock(mutex_);
    while (!done) {
        gl_job_done_cv_.wait(lock);
    }
    return status;
}

void GlContext::DedicatedThread::RunWithoutWaiting(GlVoidFunction gl_func) {
    // Note: this is invoked by GlContextExecutor. To avoid starvation of
    // non-calculator tasks in the presence of GL source calculators, calculator
    // tasks must always be scheduled as new tasks, or another solution needs to
    // be set up to avoid starvation. See b/78522434.
    CUTILS_CHECK(gl_func);
    PutJob(std::move(gl_func));
}

bool GlContext::DedicatedThread::IsCurrentThread() {
    return pthread_equal(gl_thread_id_, pthread_self());
}

bool GlContext::ParseGlVersion(std::string version, GLint *major,
                               GLint *minor) {
    size_t pos = version.find('.');
    if (pos == std::string_view::npos || pos < 1) {
        return false;
    }
    // GL_VERSION is supposed to start with the version number; see, e.g.,
    // https://www.khronos.org/registry/OpenGL-Refpages/es3/html/glGetString.xhtml
    // https://www.khronos.org/opengl/wiki/OpenGL_Context#OpenGL_version_number
    // However, in rare cases one will encounter non-conforming configurations
    // that have some prefix before the number. To deal with that, we walk
    // backwards from the dot.
    size_t start = pos - 1;
    while (start > 0 && isdigit(version[start - 1])) --start;
    *major = std::atoi(version.substr(start, (pos - start)).c_str());
    if(*major == 0) {
        return false;
    }
    auto rest = version.substr(pos + 1);
    pos = rest.find(' ');
    size_t pos2 = rest.find('.');
    if (pos == std::string_view::npos ||
        (pos2 != std::string_view::npos && pos2 < pos)) {
        pos = pos2;
    }
    *minor = std::atoi(rest.substr(0, pos).c_str());
    if(*minor == 0) {
        return false;
    }
    return true;
}

Status GlContext::FinishInitialization(bool create_thread) {
    if (create_thread) {
        thread_ = std::make_unique<GlContext::DedicatedThread>();
        auto ret =(thread_->Run([this] { return EnterContext(nullptr); }));
        CUTILS_RETURN_STATUS_IF_ERROR(ret.ok(), ret);
    } else {
        thread_.reset(nullptr);
    }

    return Run([this]() -> Status {
        std::string version_string(
                reinterpret_cast<const char *>(glGetString(GL_VERSION)));

        // Let's try getting the numeric version if possible.
        glGetIntegerv(GL_MAJOR_VERSION, &gl_major_version_);
        GLenum err = glGetError();
        if (err == GL_NO_ERROR) {
            glGetIntegerv(GL_MINOR_VERSION, &gl_minor_version_);
        } else {
            // GL_MAJOR_VERSION is not supported on GL versions below 3. We have to
            // parse the version std::string.
            if (!ParseGlVersion(version_string, &gl_major_version_,
                                &gl_minor_version_)) {
                CLOGD("invalid GL_VERSION format: ' %s; assuming 2.0", version_string.c_str());
                gl_major_version_ = 2;
                gl_minor_version_ = 0;
            }
        }

        CLOG("GL version: %d.%d (%s)", gl_major_version_, gl_minor_version_, glGetString(GL_VERSION));

        return OkStatus();
    });
}

GlContext::GlContext() {}

GlContext::~GlContext() {
    ScopedTimer _t("~GlContext");
    // Note: on Apple platforms, this object contains Objective-C objects.
    // The destructor will release them, but ARC must be on.
    if (thread_) {
        auto status = thread_->Run([this] {
            return ExitContext(nullptr);
        });
        if (!status.ok()) {
            CLOGE("Failed to deactivate context on thread: %d %s", status.code(),
                     status.message().c_str());
        }
        if (thread_->IsCurrentThread()) {
            thread_.release()->SelfDestruct();
        }
    }
    DestroyContext();
}


Status GlContext::Run(GlStatusFunction gl_func, int node_id,
                      Timestamp input_timestamp) {
    Status status;
    if (thread_) {
        bool had_gl_errors = false;
        status = thread_->Run(
                [this, gl_func, node_id, &input_timestamp, &had_gl_errors] {
                    auto status = gl_func();
                    had_gl_errors = CheckForGlErrors();
                    return status;
                });
        LogUncheckedGlErrors(had_gl_errors);
    } else {
        ContextBinding saved_context;
        auto enter = EnterContext(&saved_context);
        CUTILS_RETURN_STATUS_IF_ERROR(enter.ok(), enter);
        status = gl_func();
        LogUncheckedGlErrors(CheckForGlErrors());
        auto exit = ExitContext(&saved_context);
        CUTILS_RETURN_STATUS_IF_ERROR(exit.ok(), exit);
    }
    return status;
}

void GlContext::RunWithoutWaiting(GlVoidFunction gl_func) {
    CLOGD("run without waiting , thread:%p", thread_.get());
    if (thread_) {
        // Add ref to keep the context alive while the task is executing.
        auto context = shared_from_this();
        thread_->RunWithoutWaiting([this, context, gl_func] {
            gl_func();
            LogUncheckedGlErrors(CheckForGlErrors());
        });
    } else {
        // TODO: queue up task instead.
        ContextBinding saved_context;
        auto status = EnterContext(&saved_context);
        if (!status.ok()) {
            CLOGE("Failed to enter context: %d:%s", status.code(), status.message().c_str());
            return;
        }
        gl_func();
        LogUncheckedGlErrors(CheckForGlErrors());
        status = ExitContext(&saved_context);
        if (!status.ok()) {
            CLOGE("Failed to exit context:%d:%s", status.code(), status.message().c_str());
        }
    }
}

std::weak_ptr<GlContext> &GlContext::CurrentContext() {
    thread_local std::weak_ptr<GlContext> current_context;
    return current_context;
}

Status GlContext::SwitchContext(ContextBinding *saved_context,
                                const ContextBinding &new_context) {
    std::shared_ptr<GlContext> old_context_obj = CurrentContext().lock();
    std::shared_ptr<GlContext> new_context_obj =
            new_context.context_object.lock();
    if (saved_context) {
        saved_context->context_object = old_context_obj;
        GetCurrentContextBinding(saved_context);
    }
    // Check that the context object is consistent with the native context.
    if (old_context_obj && saved_context) {
        CUTILS_CHECK(old_context_obj->context_ == saved_context->context);
    }
    if (new_context_obj) {
        CUTILS_CHECK(new_context_obj->context_ == new_context.context);
    }

    if (new_context_obj && (old_context_obj == new_context_obj)) {
        return OkStatus();
    }

    if (old_context_obj) {
        // 1. Even if we cannot restore the new context, we want to get out of the
        // old one (we may be deliberately trying to exit it).
        // 2. We need to unset the old context before we unlock the old mutex,
        // Therefore, we first unset the old one before setting the new one.
        auto ret = SetCurrentContextBinding({});
        CUTILS_RETURN_STATUS_IF_ERROR(ret.ok(), ret);
        old_context_obj->context_use_mutex_.unlock();
        CurrentContext().reset();
    }

    if (new_context_obj) {
        new_context_obj->context_use_mutex_.lock();
        auto status = SetCurrentContextBinding(new_context);
        if (status.ok()) {
            CurrentContext() = new_context_obj;
        } else {
            new_context_obj->context_use_mutex_.unlock();
        }
        return status;
    } else {
        return SetCurrentContextBinding(new_context);
    }
}

Status GlContext::EnterContext(ContextBinding *saved_context) {
    CUTILS_CHECK(HasContext());
    return SwitchContext(saved_context, ThisContextBinding());
}

Status GlContext::ExitContext(
        const ContextBinding *saved_context) {
    ContextBinding no_context;
    if (!saved_context) {
        saved_context = &no_context;
    }
    return SwitchContext(nullptr, *saved_context);
}

std::shared_ptr<GlContext> GlContext::GetCurrent() {
    return CurrentContext().lock();
}

void GlContext::GlFinishCalled() {
    std::lock_guard<std::mutex> lock(mutex_);
    ++gl_finish_count_;
    wait_for_gl_finish_cv_.notify_all();
}

class GlFinishSyncPoint : public GlSyncPoint {
public:
    explicit GlFinishSyncPoint(const std::shared_ptr<GlContext> &gl_context)
            : GlSyncPoint(gl_context),
              gl_finish_count_(gl_context_->gl_finish_count()) {}

    void Wait() override {
        gl_context_->WaitForGlFinishCountPast(gl_finish_count_);
    }

    bool IsReady() override {
        return gl_context_->gl_finish_count() > gl_finish_count_;
    }

private:
    // Number of glFinish calls done before the creation of this token.
    int64_t gl_finish_count_ = -1;
};

class GlFenceSyncPoint : public GlSyncPoint {
public:
    explicit GlFenceSyncPoint(const std::shared_ptr<GlContext> &gl_context)
            : GlSyncPoint(gl_context) {
        gl_context_->Run([this] {
            sync_ = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
            glFlush();
        });
    }

    ~GlFenceSyncPoint() {
        if (sync_) {
            GLsync sync = sync_;
            gl_context_->RunWithoutWaiting([sync] { glDeleteSync(sync); });
        }
    }

    GlFenceSyncPoint(const GlFenceSyncPoint &) = delete;

    GlFenceSyncPoint &operator=(const GlFenceSyncPoint &) = delete;

    void Wait() override {
        if (!sync_) return;
        gl_context_->Run([this] {
            GLenum result =
                    glClientWaitSync(sync_, 0, std::numeric_limits<uint64_t>::max());
            if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
                glDeleteSync(sync_);
                sync_ = nullptr;
            }
            // TODO: do something if the wait fails?
        });
    }

    void WaitOnGpu() override {
        if (!sync_) return;
        // TODO: do not wait if we are already on the same context?
        glWaitSync(sync_, 0, GL_TIMEOUT_IGNORED);
    }

    bool IsReady() override {
        if (!sync_) return true;
        bool ready = false;
        // TODO: we should not block on the original context if possible.
        gl_context_->Run([this, &ready] {
            GLenum result = glClientWaitSync(sync_, 0, 0);
            if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
                glDeleteSync(sync_);
                sync_ = nullptr;
                ready = true;
            }
        });
        return ready;
    }

private:
    GLsync sync_;
};

void GlMultiSyncPoint::Add(std::shared_ptr<GlSyncPoint> new_sync) {
    for (auto &sync : syncs_) {
        if (&sync->GetContext() == &new_sync->GetContext()) {
            sync = std::move(new_sync);
            return;
        }
    }
    syncs_.emplace_back(std::move(new_sync));
}

void GlMultiSyncPoint::Wait() {
    for (auto &sync : syncs_) {
        sync->Wait();
    }
    // At this point all the syncs have been reached, so clear them out.
    syncs_.clear();
}

void GlMultiSyncPoint::WaitOnGpu() {
    for (auto &sync : syncs_) {
        sync->WaitOnGpu();
    }
    // TODO: when do we clear out these syncs?
}

bool GlMultiSyncPoint::IsReady() {
    syncs_.erase(
            std::remove_if(syncs_.begin(), syncs_.end(),
                           std::bind(&GlSyncPoint::IsReady, std::placeholders::_1)),
            syncs_.end());
    return syncs_.empty();
}

// Set this to 1 to disable syncing. This can be used to verify that a test
// correctly detects sync issues.
#define MEDIAPIPE_DISABLE_GL_SYNC_FOR_DEBUG 0

#if MEDIAPIPE_DISABLE_GL_SYNC_FOR_DEBUG
class GlNopSyncPoint : public GlSyncPoint {
 public:
  explicit GlNopSyncPoint(const std::shared_ptr<GlContext>& gl_context)
      : GlSyncPoint(gl_context) {}

  void Wait() override {}

  bool IsReady() override { return true; }
};
#endif

std::shared_ptr<GlSyncPoint> GlContext::CreateSyncToken() {
    std::shared_ptr<GlSyncPoint> token;
#if MEDIAPIPE_DISABLE_GL_SYNC_FOR_DEBUG
    token.reset(new GlNopSyncPoint(shared_from_this()));
#else
    if (SymbolAvailable(&glWaitSync)) {
        token.reset(new GlFenceSyncPoint(shared_from_this()));
    } else {
        token.reset(new GlFinishSyncPoint(shared_from_this()));
    }
#endif
    return token;
}

std::shared_ptr<GlSyncPoint> GlContext::TestOnly_CreateSpecificSyncToken(
        SyncTokenTypeForTest type) {
    std::shared_ptr<GlSyncPoint> token;
    switch (type) {
        case SyncTokenTypeForTest::kGlFinish:
            token.reset(new GlFinishSyncPoint(shared_from_this()));
            return token;
    }
    return nullptr;
}

void GlContext::WaitForGlFinishCountPast(int64_t count_to_pass) {
    if (gl_finish_count_ > count_to_pass) return;
    auto finish_task = [this, count_to_pass]() {
        // When a GlFinishSyncToken is created it takes the current finish count
        // from the GlContext, and we must wait for gl_finish_count_ to pass it.
        // Therefore, we need to do at most one more glFinish call. This CUTILS_CHECK
        // is used for documentation and sanity-checking purposes.
        CUTILS_CHECK(gl_finish_count_ >= count_to_pass);
        if (gl_finish_count_ == count_to_pass) {
            glFinish();
            GlFinishCalled();
        }
    };
    if (IsCurrent()) {
        // If we are already on the current context, we cannot call
        // RunWithoutWaiting, since that task will not run until this function
        // returns. Instead, call it directly.
        finish_task();
        return;
    }
    // We do not schedule this action using Run because we don't necessarily
    // want to wait for it to complete. If another job calls GlFinishCalled
    // sooner, we are done.
    RunWithoutWaiting(std::move(finish_task));
    std::unique_lock<std::mutex> lock(mutex_);
    while (gl_finish_count_ <= count_to_pass) {
        wait_for_gl_finish_cv_.wait(lock);
    }
}

void GlContext::WaitSyncToken(const std::shared_ptr<GlSyncPoint> &token) {
    CUTILS_CHECK(token);
    token->Wait();
}

bool GlContext::SyncTokenIsReady(const std::shared_ptr<GlSyncPoint> &token) {
    CUTILS_CHECK(token);
    return token->IsReady();
}

bool GlContext::CheckForGlErrors() {
    if (!HasContext()) return false;
    GLenum error;
    bool had_error = false;
    while ((error = glGetError()) != GL_NO_ERROR) {
        had_error = true;
        switch (error) {
            case GL_INVALID_ENUM:
                CLOG("Found unchecked GL error: GL_INVALID_ENUM");
                break;
            case GL_INVALID_VALUE:
                CLOG("Found unchecked GL error: GL_INVALID_VALUE");
                break;
            case GL_INVALID_OPERATION:
                CLOG("Found unchecked GL error: GL_INVALID_OPERATION");
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                CLOG("Found unchecked GL error: GL_INVALID_FRAMEBUFFER_OPERATION");
                break;
            case GL_OUT_OF_MEMORY:
                CLOG("Found unchecked GL error: GL_OUT_OF_MEMORY");
                break;
            default:
                CLOG("Found unchecked GL error: UNKNOWN ERROR");
                break;
        }
    }
    return had_error;
}

void GlContext::LogUncheckedGlErrors(bool had_gl_errors) {
    if (had_gl_errors) {
        // TODO: ideally we would print a backtrace here, or at least
        // the name of the current calculator, to make it easier to find the
        // culprit. In practice, getting a backtrace from Android without crashing
        // is nearly impossible, so screw it. Just change this to LOG(FATAL) when
        // you want to debug.
        CLOGW("Ignoring unchecked GL error.");
    }
}

template<typename T>
std::future<T> GlContext::Post(std::function<T()> gl_func) {
    std::promise<T> promise;
    auto fut = promise.get_future();
    if (thread_) {
        // Add ref to keep the context alive while the task is executing.
        auto context = shared_from_this();
        thread_->RunWithoutWaiting([this, context, gl_func, &promise] {
            promise.set_value(gl_func());
            LogUncheckedGlErrors(CheckForGlErrors());
        });
    } else {
        // TODO: queue up task instead.
        ContextBinding saved_context;
        auto status = EnterContext(&saved_context);
        if (!status.ok()) {
            CLOGE("Failed to enter context: %d:%s", status.code(), status.message().c_str());
            return;
        }
        promise.set_value(gl_func());
        LogUncheckedGlErrors(CheckForGlErrors());
        status = ExitContext(&saved_context);
        if (!status.ok()) {
            CLOGE("Failed to exit context:%d:%s", status.code(), status.message().c_str());
        }
    }
    return std::move(fut);
}
