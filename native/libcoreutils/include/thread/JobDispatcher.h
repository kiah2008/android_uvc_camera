//
// Created by zhaokai on 2021/1/13.
//

#ifndef HUMANVIDEOEFFECTS_JOBDISPATCHER_H
#define HUMANVIDEOEFFECTS_JOBDISPATCHER_H


#include "../port/base_types.h"
#include <functional>
#include <vector>
#include <deque>
#include <mutex>
#include <pthread.h>

namespace cutils {
    typedef std::function<CUTILS_STATUS()> StatusDispatchFunction;
    typedef std::function<void()> VoidDispatchFunction;

    class JobDispatcher {
    public:
        JobDispatcher();

        ~JobDispatcher();

        JobDispatcher(const JobDispatcher &) = delete;

        JobDispatcher &operator=(JobDispatcher) = delete;

        CUTILS_STATUS Run(StatusDispatchFunction func);

        void WaitUntilDone();
        void RunWithoutWaiting(VoidDispatchFunction func);

        bool IsCurrentThread();

        void SelfDestruct();

    private:
        static void *ThreadBody(void *instance);

        void ThreadBody();

        using Job = std::function<void(void)>;

        Job GetJob();
        inline void CheckComplete();

        void PutJob(Job job);

        std::mutex mutex_;
        // Used to wait for a job's completion.
        bool is_complete_=true;
        std::condition_variable job_done_cv_;
        std::condition_variable jobs_complete_cv_;
        pthread_t thread_id_;

        std::deque<Job> jobs_;
        std::condition_variable has_jobs_cv_;

        bool self_destruct_ = false;
    };
}

#endif //HUMANVIDEOEFFECTS_JOBDISPATCHER_H
