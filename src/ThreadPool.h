#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

class ThreadPool {
public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();

    template <typename Func, typename... Args>
    auto enqueue(Func&& func, Args&&... args) -> std::future<typename std::result_of<Func(Args...)>::type> {
        using return_type = typename std::result_of<Func(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
        std::future<return_type> result = task->get_future();
        {
            std::unique_lock<std::mutex> lock(tasks_mutex);
            if (stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }
            tasks.emplace([task]() { (*task)(); });
        }
        tasks_cond.notify_one();
        return result;
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex tasks_mutex;
    std::condition_variable tasks_cond;
    bool stop;

    void worker_thread();
};

#endif // THREAD_POOL_H

