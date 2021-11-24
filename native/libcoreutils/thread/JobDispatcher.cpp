//
// Created by zhaokai on 2021/1/13.
//

#include <port/clog.h>
#include <thread/JobDispatcher.h>

using namespace cutils;

static void SetThreadName(const char *name) {
    char thread_name[16];  // Linux requires names (with nul) fit in 16 chars
    strncpy(thread_name, name, sizeof(thread_name));
    thread_name[sizeof(thread_name) - 1] = '\0';
    int res = pthread_setname_np(pthread_self(), thread_name);
    CUTILS_CHECK_LOG(res != 0, "Can't set pthread names: name: \"%s\", error:%d", name, res);
}

cutils::JobDispatcher::JobDispatcher() {
    CUTILS_CHECK_EQ(pthread_create(&thread_id_, nullptr, ThreadBody, this), 0);
}

cutils::JobDispatcher::~JobDispatcher() {
    if (IsCurrentThread()) {
        CUTILS_CHECK(self_destruct_);
        CUTILS_CHECK_EQ(pthread_detach(thread_id_), 0);
    } else {
        // Give an invalid job to signal termination.
        PutJob({});
        CUTILS_CHECK_EQ(pthread_join(thread_id_, nullptr), 0);
    }
}

void cutils::JobDispatcher::SelfDestruct() {
    self_destruct_ = true;
    // Give an invalid job to signal termination.
    PutJob({});
}

cutils::JobDispatcher::Job cutils::JobDispatcher::GetJob() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (jobs_.empty()) {
        has_jobs_cv_.wait(lock);
    }
    Job job = std::move(jobs_.front());
    jobs_.pop_front();
    return job;
}

void cutils::JobDispatcher::PutJob(Job job) {
    std::lock_guard<std::mutex> lock(mutex_);
    is_complete_ = false;
    jobs_.push_back(std::move(job));
    has_jobs_cv_.notify_all();
}

void *cutils::JobDispatcher::ThreadBody(void *instance) {
    JobDispatcher *thread = static_cast<JobDispatcher *>(instance);
    thread->ThreadBody();
    return nullptr;
}


void cutils::JobDispatcher::ThreadBody() {
    SetThreadName("JobDispatcher");
    while (true) {
        Job job = GetJob();
        if (!job) {
            CheckComplete();
            CLOGE("get invalid jobs.");
            break;
        }
        job();
        CheckComplete();
    }
    if (self_destruct_) {
        delete this;
    }
}

CUTILS_STATUS cutils::JobDispatcher::Run(StatusDispatchFunction func) {
    CUTILS_CHECK(func);
    if (IsCurrentThread()) {
        return func();
    }
    bool done = false;
    CUTILS_STATUS status;
    PutJob([this, func, &done, &status]() {
        status = func();
        std::unique_lock<std::mutex> lock(mutex_);
        done = true;
        job_done_cv_.notify_all();
    });

    std::unique_lock<std::mutex> lock(mutex_);
    while (!done) {
        job_done_cv_.wait(lock);
    }
    return status;
}

void cutils::JobDispatcher::RunWithoutWaiting(VoidDispatchFunction func) {
    CUTILS_CHECK(func);
    PutJob(std::move(func));
}

bool cutils::JobDispatcher::IsCurrentThread() {
    return pthread_equal(thread_id_, pthread_self());
}

void JobDispatcher::WaitUntilDone() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (!is_complete_) {
        jobs_complete_cv_.wait(lock);
    }
}

void JobDispatcher::CheckComplete() {
    std::unique_lock<std::mutex> lock(mutex_);
    if(jobs_.empty()) {
        is_complete_ = true;
        jobs_complete_cv_.notify_all();
    }
}
