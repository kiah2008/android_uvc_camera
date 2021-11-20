//
// Created by zhaokai on 2020/6/28.
//

#ifndef COREUTILS_THREADPOOLEXECUTOR_H
#define COREUTILS_THREADPOOLEXECUTOR_H

#include <functional>
#include "Executor.h"
#include "thread_option.h"
#include "ThreadPool.h"
#include "ThreadPoolExecutorOptions.h"

namespace cutils {
    class ThreadPoolExecutor : public Executor {
    public:
        static std::shared_ptr<Executor> Create(
                const ThreadPoolExecutorOptions &extendable_options);

        explicit ThreadPoolExecutor(int num_threads);
        ~ThreadPoolExecutor() override;
        void Schedule(std::function<void()> task) override;

        // For testing.
        int num_threads() const { return thread_pool_.num_threads(); }
        // Returns the thread stack size (in bytes).
        size_t stack_size() const { return stack_size_; }

    private:
        ThreadPoolExecutor(const ThreadOptions& thread_options, int num_threads);

        // Saves the value of the stack size option and starts the thread pool.
        void Start();

        ThreadPool thread_pool_;

        // Records the stack size in ThreadOptions right before we call
        // thread_pool_.StartWorkers().
        //
        // The actual stack size passed to pthread_attr_setstacksize() for the
        // worker threads differs from the stack size we specified. It includes the
        // guard size and space for thread-local storage. (See Thread::Start() in
        // thread/thread.cc.) So the unit tests check the stack size in
        // ThreadOptions, in addition to trying to recover the specified stack
        // size from the stack size returned by pthread_getattr_np(),
        // pthread_attr_getstacksize(), and pthread_attr_getguardsize().
        size_t stack_size_ = 0;
    };
}


#endif //COREUTILS_THREADPOOLEXECUTOR_H
