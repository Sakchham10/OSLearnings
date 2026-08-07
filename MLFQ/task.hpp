//
// Created by Sakchham Sangroula on 7/30/26.
//

#ifndef OSLEARNINGS_TASK_HPP
#define OSLEARNINGS_TASK_HPP
#include <chrono>
#include <cstddef>
#include <functional>
#include <iostream>
#include <random>
#include <ratio>
enum class taskPriority {
  First = 0,
  Second = 1,
  Third = 2,
  Fourth = 3,
  Fifth = 4,
  Count = 5
};

class task {
  int id;
  std::chrono::milliseconds cpuTime;
  std::chrono::milliseconds ioTime;
  std::chrono::milliseconds cpuTimeCompleted;
  std::chrono::milliseconds ioTimeCompleted;
  std::chrono::milliseconds nextIoDuration;
  taskPriority priority;
  int getWork() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    double cpuPercent =
        ((cpuTime - cpuTimeCompleted) /
         (cpuTime - cpuTimeCompleted + ioTime - ioTimeCompleted)) *
        100.0;
    double ioProbability = (100.0 - cpuPercent) / 100.0;
    std::bernoulli_distribution dist(ioProbability);
    return dist(gen) ? 1 : 0;
  }
  void calculateNextIoDuration() {
    if (ioTimeCompleted >= ioTime) {
      nextIoDuration = static_cast<std::chrono::milliseconds>(0);
      return;
    }
    if (cpuTimeCompleted >= cpuTime) {
      nextIoDuration = (ioTime - ioTimeCompleted);
      return;
    }
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> dist(
        0, (ioTime - ioTimeCompleted).count());
    nextIoDuration = std::chrono::milliseconds(dist(gen));
  }

public:
  bool runTask(std::chrono::milliseconds quanta) {
    auto start = std::chrono::high_resolution_clock::now();
    auto shouldRunIo = false;
    while (true && cpuTimeCompleted < cpuTime) {
      auto end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double, std::milli> elapsed = end - start;
      if (elapsed >= quanta) {
        return shouldRunIo;
      }
      auto work = getWork();
      if (work == 0) {
        std::chrono::milliseconds duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - end);
        cpuTimeCompleted += duration;
      } else {
        shouldRunIo = true;
        calculateNextIoDuration();
        std::chrono::milliseconds duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - end);
        cpuTimeCompleted += duration;
        return shouldRunIo;
      }
    }
    if (ioTime > ioTimeCompleted) {
      calculateNextIoDuration();
      return true;
    }
    return false;
  }
  taskPriority getPriority() { return priority; }
  void changePriority(taskPriority newPriority) {
    if (newPriority == taskPriority::Count) {
      std::cout
          << "Priority of the task is wrong, moving to highest priority\n";
      priority = taskPriority::First;
      return;
    }
    priority = newPriority;
  }
  std::chrono::milliseconds getNextIoDuration() { return nextIoDuration; }
  task(std::function<void()> work, std::chrono::milliseconds cpu,
       std::chrono::milliseconds io)
      : cpuTime(cpu), ioTime(io), cpuTimeCompleted(0), ioTimeCompleted(0),
        nextIoDuration(0), priority(taskPriority::First) {}
};

#endif // OSLEARNINGS_TASK_HPP
