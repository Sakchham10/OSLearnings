//
// Created by Sakchham Sangroula on 7/28/26.
//

#ifndef OSLEARNINGS_MLFQ_HPP
#define OSLEARNINGS_MLFQ_HPP

#include "hashMap.hpp"
#include "queue.hpp"
#include "runnable.hpp"
#include "task.hpp"
#include "threadManager.hpp"
#include "workItem.hpp"
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <mutex>
class mlfq {
  bool shutDownRequested;
  int numWork;
  std::vector<basics::queue<basics::workItem>> queues;
  basics::hashMap<std::size_t, std::chrono::milliseconds> queueNumToQuanta;
  std::size_t priorityBoostInterval;
  basics::hashMap<int, int> taskIdToCurrentCpuTime;
  basics::threadManager manager;
  std::mutex lock;
  std::mutex queuesLock;
  std::condition_variable cv;

public:
  void pushTask(task newTask) {
    auto priority = static_cast<std::size_t>(newTask.getPriority());
    std::unique_lock<std::mutex> shutDownLock(lock);
    if (shutDownRequested) {
      std::cout << "Shut down has been requested, can't add more tasks\n";
      return;
    }
    numWork++;
    auto timeQuanta = queueNumToQuanta.get(priority);
    if (!timeQuanta.has_value()) {
      std::cout << "No time quantum for priority\n";
      return;
    }
    auto workItem = basics::workItem(std::make_unique<task>(std::move(newTask)),
                                     timeQuanta.value());
    queues[priority].append(std::move(workItem));
    shutDownLock.unlock();
    cv.notify_all();
  }
  void run() {
    while (true) {
      std::optional<basics::workItem> nextTask;
      {
        std::unique_lock<std::mutex> shutDownLock(lock);
        cv.wait(shutDownLock,
                [this]() { return shutDownRequested || numWork > 0; });
        if (shutDownRequested && numWork == 0) {
          return;
        }
        for (auto i = 0; i < queues.size(); i++) {
          auto &queueAt = queues[i];
          if (queueAt.getSize() > 0) {
            nextTask.emplace(queueAt.popLeft());
            numWork--;
            break;
          }
        }
      }
      if (nextTask.has_value()) {
        manager.submit(std::move(nextTask.value()));
      }
    }
  };
  mlfq(std::size_t timeQMs = 100, std::size_t priorityBoostMs = 300)
      : shutDownRequested(false), numWork(0),
        queues(static_cast<std::size_t>(basics::runnablePriority::Count)),
        priorityBoostInterval(priorityBoostMs) {}
};

#endif // OSLEARNINGS_MLFQ_HPP
