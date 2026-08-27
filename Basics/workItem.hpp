#ifndef BASICS_WORKITEM_HPP
#define BASICS_WORKITEM_HPP

#include "runnable.hpp"
#include <any>
#include <chrono>
#include <future>
#include <memory>

namespace basics {

class workItem {
public:
  std::unique_ptr<runnable> task;
  std::promise<std::any> result;
  std::chrono::milliseconds quantum;
  workItem(workItem &&) = default;
  workItem() = delete;
  explicit workItem(std::unique_ptr<runnable> task,
                    std::chrono::milliseconds quantum)
      : task(std::move(task)), quantum(quantum) {};
};

} // namespace basics

#endif // BASICS_WORKITEM_HPP
