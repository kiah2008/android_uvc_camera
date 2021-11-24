//
// Created by zhaokai on 2020/7/14.
//

#ifndef HUMANVIDEOEFFECTS_GLCONTEXT_INTERNAL_H
#define HUMANVIDEOEFFECTS_GLCONTEXT_INTERNAL_H

#include <deque>
#include <string>
#include <pthread.h>
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <gpu/GLContext.h>

namespace cutils{
    class GlContext::DedicatedThread {
    public:
        DedicatedThread();
        ~DedicatedThread();
        DedicatedThread(const DedicatedThread&) = delete;
        DedicatedThread& operator=(DedicatedThread) = delete;

        Status Run(GlStatusFunction gl_func);
        void RunWithoutWaiting(GlVoidFunction gl_fund);

        bool IsCurrentThread();

        void SelfDestruct();

    private:
        static void* ThreadBody(void* instance);
        void ThreadBody();

        using Job = std::function<void(void)>;
        Job GetJob();
        void PutJob(Job job);

        std::mutex mutex_;
        // Used to wait for a job's completion.
        std::condition_variable gl_job_done_cv_ ;
        pthread_t gl_thread_id_;

        std::deque<Job> jobs_ ;
        std::condition_variable has_jobs_cv_ ;

        bool self_destruct_ = false;
    };
}
#endif //HUMANVIDEOEFFECTS_GLCONTEXT_INTERNAL_H
