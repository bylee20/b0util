#include "thread_pool.hpp"
#include <algorithm>

namespace b0 {

struct thread_pool::data {
    std::mutex mutex;
    std::condition_variable var;
    int count = 0;
    std::deque<std::unique_ptr<job_t>> queue;
    std::vector<std::thread> threads;
    bool quit = false;

    auto make_thread(std::unique_ptr<job_t> &&_job) -> std::thread
    {
        return std::thread([this, job=std::move(_job)] () mutable {
            while(!quit) {
                job->invoke();
                if (quit)
                    break;
                std::unique_lock<std::mutex> lock(mutex);
                var.wait(lock, [this] { return !queue.empty() || quit; });
                if (quit)
                    break;
                job.swap(queue.front());
                queue.pop_front();
            }
        });
    }
    auto enqueue(std::unique_ptr<job_t> &&job) -> void
    {
        if (count <= 0) {

        }
        std::unique_lock<std::mutex> lock(mutex);
        if (threads.size() < static_cast<std::size_t>(count))
            threads.push_back(make_thread(std::move(job)));
        else {
            queue.push_back(std::move(job));
            lock.unlock();
            var.notify_one();
        }
    }
};

thread_pool::thread_pool()
    : thread_pool(-1)
{

}

thread_pool::thread_pool(int count)
    : d(new data)
{
    d->count = count;
}

thread_pool::~thread_pool() {
    d->quit = true;
    d->var.notify_all();
    for (auto &t : d->threads)
        t.join();
}

auto thread_pool::ideal_size() -> int
{
    return std::max<int>(1, std::thread::hardware_concurrency());
}

auto thread_pool::size() const -> int
{
    return d->count;
}

auto thread_pool::_enqueue(std::unique_ptr<job_t> &&job) -> void
{
    d->enqueue(std::move(job));
}

//class semaphore {
//public:
//    semaphore(int n): m_count(n) { }
//    auto acquire() -> int
//    {
//        std::unique_lock<std::mutex> lock(m_mutex);
//        m_var.wait(lock, [this] { return m_count > 0; });
//        return --m_count;
//    }
//    auto release() -> void
//    {
//        std::unique_lock<std::mutex> lock(m_mutex);
//        ++m_count;
//        m_var.notify_one();
//    }
//private:
//    int m_count = 0;
//    std::mutex m_mutex;
//    std::condition_variable m_var;
//};

}
