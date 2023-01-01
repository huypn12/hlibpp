#ifndef __MYLIBPP_THREAD_POOL_H__
#define __MYLIBPP_THREAD_POOL_H__

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace Mylibpp {

class ThreadPool {
private:
  template <typename T> class _MyThreadsafeQueue {
  private:
    std::queue<T> q_;
    std::mutex qlock_;

  public:
    void Push(T &t);
    void Push(T &&t);
  };

  std::atomic<bool> force_stop_ = false;

  int pool_size_;
  std::vector<std::thread> pool_container_;

  std::condition_variable task_condition_;
  std::mutex task_queue_lock_;
  std::queue<std::function<void()>> task_queue_;

protected:
  void RunThreadLoop() {
    std::function<void()> task;
    while (true) {
      {
        std::unique_lock<std::mutex> lock(task_queue_lock_);

        task_condition_.wait(
            lock, [this]() { return !task_queue_.empty() | force_stop_; });

        if (force_stop_)
          return;

        task = task_queue_.front();

        task_queue_.pop();
      }

      if (task) {
        task();
      }
    }
  }

public:
  ThreadPool(int pool_size)
      : pool_size_(pool_size), pool_container_(pool_size) {
    Init();
  }

  ThreadPool()
      : pool_size_(std::thread::hardware_concurrency()),
        pool_container_(pool_size_) {
    Init();
  }

  void Init() {
    for (auto i = 0; i < pool_size_; ++i) {
      pool_container_.push_back(std::thread(&ThreadPool::RunThreadLoop, this));
    }
  }

  ~ThreadPool() { Shutdown(); }

  void Shutdown() {
    force_stop_ = true;
    task_condition_.notify_all();
    for (auto &thread : pool_container_) {
      if (thread.joinable()) {
        thread.join();
      }
    }
    pool_container_.clear();
  }

  size_t GetTaskCount() {
    std::unique_lock<std::mutex> lock(task_queue_lock_);
    return task_queue_.size();
  }

  template <typename TFunc, typename... TArgs>
  auto SubmitTask(TFunc &&func, TArgs &&...args)
      -> std::future<decltype(func(args...))> {
    std::shared_ptr<std::packaged_task<decltype(func(args...))()>> task_ptr =
        nullptr;
    {
      std::unique_lock<std::mutex> lock(task_queue_lock_);
      std::function<decltype(func(args...))()> f =
          std::bind(std::forward<TFunc>(func), std::forward<TArgs>(args)...);
      task_ptr =
          std::make_shared<std::packaged_task<decltype(func(args...))()>>(f);

      task_queue_.push([task_ptr]() { (*task_ptr)(); });
    }

    task_condition_.notify_one();

    return task_ptr->get_future();
  }

  template <typename TFunc, typename... TArgs>
  auto SubmitTask(const TFunc &func, const TArgs &...args)
      -> std::future<decltype(func(args...))> {
    std::shared_ptr<std::packaged_task<decltype(func(args...))()>> task_ptr =
        nullptr;
    {
      std::unique_lock<std::mutex> lock(task_queue_lock_);
      std::function<decltype(func(args...))()> f = std::bind(func, args...);
      task_ptr =
          std::make_shared<std::packaged_task<decltype(func(args...))()>>(f);

      task_queue_.push([task_ptr]() { (*task_ptr)(); });
    }

    task_condition_.notify_one();

    return task_ptr->get_future();
  }

  void SyncThreads() {}
}; // namespace MyThreadPool
}; // namespace Mylibpp

#endif