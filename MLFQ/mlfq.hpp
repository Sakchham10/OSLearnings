//
// Created by Sakchham Sangroula on 7/28/26.
//

#ifndef OSLEARNINGS_MLFQ_HPP
#define OSLEARNINGS_MLFQ_HPP

#include "queue.hpp"
#include "task.hpp"
#include "threadManager.hpp"
#include <cstddef>
#include <map>
template <typename T> class mlfq {
  std::vector<basics::queue<task>> queues;
  std::mutex lock;
  std::size_t timeQuanta;
  std::size_t priorityBoostInterval;
  std::map<int, int> taskIdToCurrentCpuTime;
  basics::threadManager<task> manager;

public:
  void pushTask(task newTask) {
    auto priority = static_cast<std::size_t>(newTask.getPriority());
    queues[priority].append(std::move(newTask));
  }
  mlfq() : queues(static_cast<std::size_t>(taskPriority::Count)) {}
};

#endif // OSLEARNINGS_MLFQ_HPP
