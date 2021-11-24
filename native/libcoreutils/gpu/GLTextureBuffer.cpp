//
// Created by zhaokai on 2020/7/14.
//

#include <gpu/glTextureBuffer.h>
#include <port/clog.h>
#include <gpu/gpu_debug_helper.h>

using namespace cutils;

std::unique_ptr<GlTextureBuffer> GlTextureBuffer::Wrap(
        GLenum target, GLuint name, int width, int height, GpuBufferFormat format,
        DeletionCallback deletion_callback) {
    return std::make_unique<GlTextureBuffer>(target, name, width, height, format,
                                             deletion_callback);
}

std::unique_ptr<GlTextureBuffer> GlTextureBuffer::Create(int width, int height,
                                                         GpuBufferFormat format,
                                                         const void *data) {
    auto buf = std::make_unique<GlTextureBuffer>(GL_TEXTURE_2D, 0, width, height,
                                                 format, nullptr);
    if (!buf->CreateInternal(data)) {
        CLOGW("failed to create texture buf.");
        return nullptr;
    }
    return buf;
}

GlTextureBuffer::GlTextureBuffer(GLenum target, GLuint name, int width,
                                 int height, GpuBufferFormat format,
                                 DeletionCallback deletion_callback)
        : name_(name),
          width_(width),
          height_(height),
          format_(format),
          target_(target),
          deletion_callback_(deletion_callback) {}

bool GlTextureBuffer::CreateInternal(const void *data) {
    auto context = GlContext::GetCurrent();
    if (!context) {
        //not using GLContext
        if(eglGetCurrentContext() == EGL_NO_CONTEXT) {
            CLOGE("not work on gl thread.");
            return false;
        }
    }

    glGenTextures(1, &name_);
    if (!name_) {
        CUTILS_CHECK_FOR_GL_ERROR();
        return false;
    }

    glBindTexture(target_, name_);
    glTexParameteri(target_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GlTextureInfo info = GlTextureInfoForGpuBufferFormat(format_, 0);

    if (info.gl_internal_format == GL_RGBA16F &&
        SymbolAvailable(&glTexStorage2D)) {
        CUTILS_ASSERT_LOG(data == nullptr, "unimplemented");
        glTexStorage2D(target_, 1, info.gl_internal_format, width_, height_);
    } else {
        glTexImage2D(target_, 0 /* level */, info.gl_internal_format, width_,
                     height_, 0 /* border */, info.gl_format, info.gl_type, data);
    }

    glBindTexture(target_, 0);

    // Use the deletion callback to delete the texture on the context
    // that created it.
    CUTILS_CHECK(!deletion_callback_);
    deletion_callback_ = [this,
            context](std::shared_ptr<GlSyncPoint> sync_token) {
        CUTILS_CHECK_NE(name_, 0);
        GLuint name_to_delete = name_;

        auto to_release= [name_to_delete, sync_token]() {
            // TODO: maybe we do not actually have to wait for the
            // consumer sync here. Check docs.
            sync_token->WaitOnGpu();
            CUTILS_CHECK_LOG(glIsTexture(name_to_delete),
                          "Deleting invalid texture id: %d", name_to_delete);
            glDeleteTextures(1, &name_to_delete);
        };

        if (context) {
            //using GLContext first
            context->RunWithoutWaiting(to_release);
        } else {
            ///TODO:
//            CUTILS_CHECK(eglGetCurrentContext() == EGL_NO_CONTEXT);
            if (eglGetCurrentContext() == EGL_NO_CONTEXT) CLOGE("glerror release without context.");
            to_release();
        }

    };

    return true;
}

void GlTextureBuffer::Reuse() {
    WaitForConsumersOnGpu();
    // TODO: should we just do this inside WaitForConsumersOnGpu?
    // if we do that, WaitForConsumersOnGpu can be called only once.
    consumer_multi_sync_ = std::make_unique<GlMultiSyncPoint>();
    // Reset the token.
    producer_sync_ = nullptr;
}

void GlTextureBuffer::Updated(std::shared_ptr<GlSyncPoint> prod_token) {
    CUTILS_ASSERT_LOG(!producer_sync_,
                   "Updated existing texture which had not been marked for reuse!");
    producer_sync_ = std::move(prod_token);
}

void GlTextureBuffer::DidRead(std::shared_ptr<GlSyncPoint> cons_token) {
    consumer_multi_sync_->Add(std::move(cons_token));
}

GlTextureBuffer::~GlTextureBuffer() {
    if (deletion_callback_) {
        deletion_callback_(std::move(consumer_multi_sync_));
    }
}

void GlTextureBuffer::WaitUntilComplete() {
    // Buffers created by the application (using the constructor that wraps an
    // existing texture) have no sync token and are assumed to be already
    // complete.
    if (producer_sync_) {
        producer_sync_->Wait();
    }
}

void GlTextureBuffer::WaitOnGpu() {
    // Buffers created by the application (using the constructor that wraps an
    // existing texture) have no sync token and are assumed to be already
    // complete.
    if (producer_sync_) {
        producer_sync_->WaitOnGpu();
    }
}

void GlTextureBuffer::WaitForConsumers() { consumer_multi_sync_->Wait(); }

void GlTextureBuffer::WaitForConsumersOnGpu() {
    consumer_multi_sync_->WaitOnGpu();
}


