#ifndef BASICS_WORKITEM_HPP
#define BASICS_WORKITEM_HPP

#include "task.hpp"
#include <chrono>
#include <memory>

enum class runnablePriority {
  First = 0,
  Second = 1,
  Third = 2,
  Fourth = 3,
  Fifth = 4,
  Count = 5
};

class workItem {
public:
  std::unique_ptr<task> runnable;
  std::chrono::milliseconds quantum;
  workItem(workItem &&) = default;
  workItem() = delete;
  explicit workItem(std::unique_ptr<task> task,
                    std::chrono::milliseconds quantum)
      : runnable(std::move(task)), quantum(quantum) {};
};

#endif // BASICS_WORKITEM_HPP
