#ifndef BASICS_THREADMANAGER_HPP
#define BASICS_THREADMANAGER_HPP
#include "queue.hpp"
#include <algorithm>
#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

namespace basics {

template <typename T> class threadManager {
  bool shuttingDown;
  std::vector<std::thread> threads;
  int threadCount;
  queue<T> workQueue;
  std::mutex lock;
  std::condition_variable cv;
  std::size_t maxSize;
  void threadTask() {
    T task;
    while (true) {
      {
        std::unique_lock<std::mutex> queueAccessLock(lock);
        cv.wait(queueAccessLock,
                [this]() { return shuttingDown || workQueue.getSize() > 0; });
        if (shuttingDown && workQueue.getSize() == 0) {
          return;
        }
        task = workQueue.popLeft();
      }
      cv.notify_all();
      try {
        task.runTask();
      } catch (...) {
        std::cout << "Could not perform an operation.\n";
      }
    }
  }; // namespace basics

public:
  ~threadManager() {
    {
      std::unique_lock<std::mutex> queueAccessLock(lock);
      shuttingDown = true;
    }
    cv.notify_all();
    for (auto &thread : threads) {
      thread.join();
    };
  };
  threadManager(int count = 10)
      : shuttingDown(false), threads(std::vector<std::thread>()),
        threadCount(count), maxSize(30) {
    threads.reserve(threadCount);
    for (auto i = 0; i < threadCount; i++) {
      threads.emplace_back(&threadManager::threadTask, this);
    }
  }
  void submit(T task) {
    std::unique_lock<std::mutex> queueAcessLock(lock);
    cv.wait(queueAcessLock,
            [this]() { return shuttingDown || workQueue.getSize() < maxSize; });
    if (shuttingDown) {
      std::cout << "Shut down has been requested, can't add more tasks\n";
      return;
    }
    workQueue.append(std::move(task));
    queueAcessLock.unlock();
    cv.notify_one();
  }
};

} // namespace basics

#endif
