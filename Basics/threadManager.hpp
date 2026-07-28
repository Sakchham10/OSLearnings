#ifndef BASICS_THREADMANAGER_HPP
#define BASICS_THREADMANAGER_HPP
#include "queue.hpp"
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

namespace basics {

template <typename T> class threadManager {
  bool shuttingDown;
  std::vector<std::thread> threads;
  int threadCount;
  queue<std::function<T>> workQueue;
  std::mutex lock;
  std::condition_variable cv;
  void threadTask() {
    while (true) {
      std::function<T> task;
      {
        std::unique_lock<std::mutex> queueAccessLock(lock);
        cv.wait(queueAccessLock,
                [this]() { return shuttingDown || workQueue.getSize() > 0; });
        if (shuttingDown && workQueue.getSize() == 0) {
          return;
        }
        task = workQueue.popLeft();
      }
      try {
        task();
      } catch (...) {
        std::cout << "Could not perform an operation.\n";
      }
    }
  };

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
        threadCount(count) {
    threads.reserve(threadCount);
    for (auto i = 0; i < threadCount; i++) {
      threads.emplace_back(&threadManager::threadTask, this);
    }
  }
};

} // namespace basics

#endif
