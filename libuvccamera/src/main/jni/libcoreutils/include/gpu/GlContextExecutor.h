//
// Created by zhaokai on 2020/7/14.
//

#ifndef UVCCAMERA_MASTER_GLCONTEXTEXECUTOR_H
#define UVCCAMERA_MASTER_GLCONTEXTEXECUTOR_H

#include <gpu/GLContext.h>
#include <thread/Executor.h>

namespace cutils {
    class GlContextExecutor : public Executor {
    public:
        explicit GlContextExecutor(GlContext *gl_context) : gl_context_(gl_context) {}

        ~GlContextExecutor() override {}

        void Schedule(std::function<void()> task) override {
            gl_context_->RunWithoutWaiting(std::move(task));
        }

    private:
        GlContext *const gl_context_;
    };


}


#endif //UVCCAMERA_MASTER_GLCONTEXTEXECUTOR_H
