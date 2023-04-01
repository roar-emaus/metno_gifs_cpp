#include "ThreadPool.h"


ThreadPool::ThreadPool(size_t num_threads)
    : stop(false) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back([this] { worker_thread(); });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        stop = true;
    }
    tasks_cond.notify_all();
    for (auto &worker : workers) {
        worker.join();
    }
}

void ThreadPool::worker_thread() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(tasks_mutex);
            tasks_cond.wait(lock, [this] { return stop || !tasks.empty(); });

            if (stop && tasks.empty()) {
                return;
            }

            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
    }
}

