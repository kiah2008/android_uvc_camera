//
// Created by zhaokai on 2020/7/14.
//

#ifndef HUMANVIDEOEFFECTS_GLTEXTUREBUFFER_H
#define HUMANVIDEOEFFECTS_GLTEXTUREBUFFER_H

#include <functional>
#include "gl_base.h"
#include "GLContext.h"
#include "gpu_buffer_format.h"

namespace cutils {
    class GlCalculatorHelperImpl;

// Implements a GPU memory buffer as an OpenGL texture. For internal use.
    class GlTextureBuffer {
    public:
        // This is called when the texture buffer is deleted. It is passed a sync
        // token created at that time on the GlContext. If the GlTextureBuffer has
        // been created from a texture not owned by MediaPipe, the sync token can be
        // used to wait until a point when it is certain that MediaPipe's GPU tasks
        // are done reading from the texture. This is improtant if the code outside
        // of MediaPipe is going to reuse the texture.
        using DeletionCallback =
        std::function<void(std::shared_ptr<GlSyncPoint> sync_token)>;

        // Wraps an existing texture, but does not take ownership of it.
        // deletion_callback is invoked when the GlTextureBuffer is released, so
        // the caller knows that the texture is no longer in use.
        // The commands producing the texture are assumed to be completed at the
        // time of this call. If not, call Updated on the result.
        static std::unique_ptr<GlTextureBuffer> Wrap(
                GLenum target, GLuint name, int width, int height, GpuBufferFormat format,
                DeletionCallback deletion_callback);

        // Creates a texture of dimensions width x height and allocates space for it.
        // If data is provided, it is uploaded to the texture; otherwise, it can be
        // provided later via glTexSubImage2D.
        static std::unique_ptr<GlTextureBuffer> Create(int width, int height,
                                                       GpuBufferFormat format,
                                                       const void *data = nullptr);

        // Wraps an existing texture, but does not take ownership of it.
        // deletion_callback is invoked when the GlTextureBuffer is released, so
        // the caller knows that the texture is no longer in use.
        // The commands producing the texture are assumed to be completed at the
        // time of this call. If not, call Updated on the result.
        GlTextureBuffer(GLenum target, GLuint name, int width, int height,
                        GpuBufferFormat format, DeletionCallback deletion_callback);

        ~GlTextureBuffer();

        // TODO: turn into a single call?
        inline GLuint name() const { return name_; }

        inline GLenum target() const { return target_; }

        inline int width() const { return width_; }

        inline int height() const { return height_; }

        inline GpuBufferFormat format() const { return format_; }

        // If this texture is going to be used outside of the context that produced
        // it, this method should be called to ensure that its updated contents are
        // available. When this method returns, all changed made before the call to
        // Updated have become visible.
        // This is necessary because texture changes are not synchronized across
        // contexts in a sharegroup.
        // NOTE: This blocks the current CPU thread and makes the changes visible
        // to the CPU. If you want to access the data via OpenGL, use WaitOnGpu
        // instead.
        void WaitUntilComplete();

        // Call this method to synchronize the current GL context with the texture's
        // producer. This will not block the current CPU thread, but will ensure that
        // subsequent GL commands see the texture in its complete status, with all
        // rendering done on the GPU by the generating context.
        void WaitOnGpu();

        // Informs the buffer that its contents are going to be overwritten.
        // This invalidates the current sync token.
        // NOTE: this must be called on the context that will become the new
        // producer.
        void Reuse();

        // Informs the buffer that its contents have been updated.
        // The provided sync token marks the point when the producer has finished
        // writing the new contents.
        void Updated(std::shared_ptr<GlSyncPoint> prod_token);

        // Informs the buffer that a consumer has finished reading from it.
        void DidRead(std::shared_ptr<GlSyncPoint> cons_token);

        // Waits for all pending consumers to finish accessing the current content
        // of the texture. This (preferably the OnGpu version) should be called
        // before overwriting the texture's contents.
        void WaitForConsumers();

        void WaitForConsumersOnGpu();

    private:
        // Creates a texture of dimensions width x height and allocates space for it.
        // If data is provided, it is uploaded to the texture; otherwise, it can be
        // provided later via glTexSubImage2D.
        // Returns true on success.
        bool CreateInternal(const void *data = nullptr);

        friend class GlCalculatorHelperImpl;

        GLuint name_ = 0;
        int width_ = 0;
        int height_ = 0;
        GpuBufferFormat format_ = GpuBufferFormat::kUnknown;
        GLenum target_ = GL_TEXTURE_2D;
        // Token tracking changes to this texture. Used by WaitUntilComplete.
        std::shared_ptr<GlSyncPoint> producer_sync_;
        // Tokens tracking the point when consumers finished using this texture.
        std::unique_ptr<GlMultiSyncPoint> consumer_multi_sync_ =
                std::make_unique<GlMultiSyncPoint>();
        DeletionCallback deletion_callback_;
    };

    using GlTextureBufferSharedPtr = std::shared_ptr<GlTextureBuffer>;

}
#endif //HUMANVIDEOEFFECTS_GLTEXTUREBUFFER_H
