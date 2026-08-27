//
// Created by Sakchham Sangroula on 8/10/26.
//

#ifndef OSLEARNINGS_RUNNABLE_HPP
#define OSLEARNINGS_RUNNABLE_HPP

#include <chrono>

namespace basics {
enum class runnablePriority {
  First = 0,
  Second = 1,
  Third = 2,
  Fourth = 3,
  Fifth = 4,
  Count = 5
};

class runnable {
public:
  virtual ~runnable() = default;
  virtual int getId() = 0;
  virtual bool runTask(std::chrono::milliseconds quantum) = 0;
  virtual runnablePriority getPriority() = 0;
  virtual void changePriority(runnablePriority priority) = 0;
};
} // namespace basics
#endif // OSLEARNINGS_RUNNABLE_HPP
