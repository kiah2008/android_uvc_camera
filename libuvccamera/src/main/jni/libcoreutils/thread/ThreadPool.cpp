//
// Created by zhaokai on 2020/6/28.
//

#include <unistd.h>
#include <sys/syscall.h>
#include <thread/ThreadPool.h>
#include <port/clog.h>
#include <utils/str_helper.h>
#include <cpu/cpu_manipulate.h>

using namespace cutils;

class ThreadPool::WorkerThread {
public:
    // Creates and starts a thread that runs pool->RunWorker().
    WorkerThread(ThreadPool *pool, const std::string &name_prefix);

    // REQUIRES: Join() must have been called.
    ~WorkerThread();

    // Joins with the running thread.
    void Join();

private:
    static void *ThreadBody(void *arg);

    ThreadPool *pool_;
    std::string name_prefix_;
    pthread_t thread_;
};

ThreadPool::WorkerThread::WorkerThread(ThreadPool *pool,
                                       const std::string &name_prefix)
        : pool_(pool), name_prefix_(name_prefix) {
    pthread_create(&thread_, nullptr, ThreadBody, this);
}

ThreadPool::WorkerThread::~WorkerThread() {}

void ThreadPool::WorkerThread::Join() { pthread_join(thread_, nullptr); }

void *ThreadPool::WorkerThread::ThreadBody(void *arg) {
    auto thread = reinterpret_cast<WorkerThread *>(arg);
    int nice_priority_level =
            thread->pool_->thread_options().nice_priority_level();
    const std::set<int> selected_cpus = thread->pool_->thread_options().cpu_set();
    const std::string name =
            internal::CreateThreadName(thread->name_prefix_, syscall(SYS_gettid));
#if defined(__linux__)
    if (nice_priority_level != 0) {
        if (nice(nice_priority_level) != -1 || errno == 0) {
            FSP_LOGD("Changed the nice priority level by %d ", nice_priority_level);
        } else {
            FSP_LOGW("Error : %s, fail to change nice priority %d", strerror(errno),
                    nice_priority_level);
        }
    }
    if (/*!selected_cpus.empty()*/true) {
#if 0
        if(nice_priority_level <= -10) {
            bind_core_and_set_priority();
            FSP_LOGD("Pinned %s to great core", name.c_str());
        } else if (nice_priority_level >= 10) {
            set_current_thread_affinity_mask(cutils::SMALL_CORE);
            FSP_LOGD("Pinned %s to small core", name.c_str());
        }
#endif
    /*    for (const int cpu : selected_cpus) {
            cpu_mask += 1 << cpu;
        }
        FSP_LOGD("cpu mask %x", cpu_mask);*/
/*
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        for (const int cpu : selected_cpus) {
            CPU_SET(cpu, &cpu_set);
        }
        if (sched_setaffinity(syscall(SYS_gettid), sizeof(cpu_set_t), &cpu_set) !=
            -1 ||
            errno == 0) {*/
/*        if (syscall(__NR_sched_setaffinity, tid, sizeof(cpu_mask), &cpu_mask) != 0 ||
                errno == 0) {
            FSP_LOG("Pinned %s to core %s", name.c_str(),
                    StrJoin<const std::set<int>>(selected_cpus, ", ").c_str());
        } else {
            FSP_LOG("Error : %s"
                    ".Failed to set affinity. %s", strerror(errno),
                    StrJoin<const std::set<int>>(selected_cpus, ", ").c_str());
        }*/
    }
    int error = pthread_setname_np(pthread_self(), name.c_str());
    if (error != 0) {
        FSP_LOGD("Error : %s, Failed to set name for thread: %s", strerror(error), name.c_str());
    }
#else
    if (nice_priority_level != 0 || !selected_cpus.empty()) {
        FSP_LOG("Thread priority and processor affinity feature aren't "
                "supported on the current platform.");
    }
    int error = pthread_setname_np(name.c_str());
    if (error != 0) {
        FSP_LOG("Error : %s Failed to set name for thread: %s", strerror(error), name);
    }
#endif
    thread->pool_->RunWorker();
    return nullptr;
}

/*    ThreadPool::ThreadPool(const std::string &name, const ThreadOptions &threadOption, int threadNum,
                       std::string name_prefix) : executor_name_(name), thread_option_(
            threadOption), thread_num_(threadNum) {
        if (thread_num_ <= 0) {
            thread_num_ = 1;
        }
        if (name_prefix.empty()) {
            name_prefix_ = "executor/" + name;
        }
    }*/

void ThreadPool::RunWorker() {
    std::unique_lock<std::mutex> _l(mutex_);
    while (true) {
//        FSP_LOGD("RunWorker %zu %p", tasks_.size(), this);
        if (!tasks_.empty()) {
            std::function<void()> task = std::move(tasks_.front());
            tasks_.pop_front();
            _l.unlock();
            task();
            _l.lock();
        } else {
            if (stopped_) {
//                FSP_LOGD("RunWorker stopped!");
                break;
            } else {
                condition_.wait(_l);
            }
        }
    }
}

const ThreadOptions &ThreadPool::thread_options() const {
    return thread_option_;
}

void ThreadPool::Schedule(FUNC func) {
//    FSP_LOGD("Schedule %p", this);
    {
        std::unique_lock<std::mutex> _l(mutex_);
        tasks_.push_back(std::move(func));
    }
    condition_.notify_one();
}

std::string internal::CreateThreadName(const std::string &prefix, int thread_id) {
    char names[16];
    // 16 is the limit allowed by `pthread_setname_np`, including
    // the terminating null byte ('\0')
    snprintf(names, 16, "%s/%d", prefix.c_str(), thread_id);
    return names;
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> _l(mutex_);
        stopped_ = true;
    }
    condition_.notify_all();

    for (int i = 0; i < threads_.size(); ++i) {
        threads_[i]->Join();
        delete threads_[i];
    }

    threads_.clear();
}

void ThreadPool::StartWorkers() {
    for (int i = 0; i < num_threads_; ++i) {
        threads_.push_back(new WorkerThread(this, name_prefix_));
    }
}

ThreadPool::ThreadPool(int num_threads) {
    num_threads_ = (num_threads <= 0) ? 1 : num_threads;
}

ThreadPool::ThreadPool(const std::string &name_prefix, int num_threads) : name_prefix_(
        name_prefix) {
    num_threads_ = (num_threads <= 0) ? 1 : num_threads;
}

ThreadPool::ThreadPool(const ThreadOptions &thread_options, const std::string &name_prefix,
                       int num_threads) : thread_option_(thread_options),name_prefix_(name_prefix) {
    num_threads_ = (num_threads <= 0) ? 1 : num_threads;
}