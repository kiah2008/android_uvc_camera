//
// Created by zhaokai on 2020/6/28.
//

#ifndef COREUTILS_THREADPOOL_H
#define COREUTILS_THREADPOOL_H

#include "Executor.h"

namespace cutils {
    class ThreadPool {
    public:
        typedef std::function<void()> FUNC;

        // Create a thread pool that provides a concurrency of "num_threads"
        // threads. I.e., if "num_threads" items are added, they are all
        // guaranteed to run concurrently without excessive delay.
        // It has an effectively infinite maximum queue length.
        // If num_threads is 1, the callbacks are run in FIFO order.
        explicit ThreadPool(int num_threads);

        ThreadPool(const ThreadPool &) = delete;

        ThreadPool &operator=(const ThreadPool &) = delete;

        // Like the ThreadPool(int num_threads) constructor, except that
        // it also associates "name_prefix" with each of the threads
        // in the thread pool.
        ThreadPool(const std::string &name_prefix, int num_threads);

        // Create a thread pool that creates and can use up to "num_threads"
        // threads.  Any standard thread options, such as stack size, should
        // be passed via "thread_options".  "name_prefix" specifies the
        // thread name prefix.
        ThreadPool(const ThreadOptions &thread_options,
                   const std::string &name_prefix, int num_threads);

        virtual ~ThreadPool();

        void Schedule(FUNC func);

        inline const int num_threads() const {
            return num_threads_;
        }

        void StartWorkers();

        const ThreadOptions &thread_options() const;

    private:
        class WorkerThread;

        void RunWorker();

        std::vector<WorkerThread *> threads_;
        int num_threads_;

        std::mutex mutex_;
        std::condition_variable condition_;
        bool stopped_ = false;
        std::deque<FUNC> tasks_;

        ThreadOptions thread_option_;

        std::string name_prefix_;
    };

    namespace internal {

        // Creates name for thread in a thread pool based on provided prefix and
        // thread id. Length of the resulting name is guaranteed to be less or equal
        // to 15. Name or thread id can be truncated to achieve that, see truncation
        // samples below:
        // name_prefix, 1234       -> name_prefix/123
        // name_prefix, 1234567    -> name_prefix/123
        // name_prefix_long, 1234  -> name_prefix_lon
        std::string CreateThreadName(const std::string &prefix, int thread_id);

    }  // namespace internal
}


#endif //COREUTILS_THREADPOOL_H
