//
// Created by Sakchham Sangroula on 7/30/26.
//

#ifndef OSLEARNINGS_TASK_HPP
#define OSLEARNINGS_TASK_HPP
#include "runnable.hpp"
#include <chrono>
#include <cstddef>
#include <iostream>
#include <random>
#include <ratio>

class task : public basics::runnable {
  int id;
  std::chrono::milliseconds cpuTime;
  std::chrono::milliseconds ioTime;
  std::chrono::milliseconds cpuTimeCompleted;
  std::chrono::milliseconds ioTimeCompleted;
  std::chrono::milliseconds nextIoDuration;
  basics::runnablePriority priority;
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
  // TODO: make move constructor, see mlfq.hpp (std::move(task))
  bool runTask(std::chrono::milliseconds quanta) override {
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
  basics::runnablePriority getPriority() override { return priority; }
  void changePriority(basics::runnablePriority newPriority) override {
    if (newPriority == basics::runnablePriority::Count) {
      std::cout
          << "Priority of the task is wrong, moving to highest priority\n";
      priority = basics::runnablePriority::First;
      return;
    }
    priority = newPriority;
  }
  std::chrono::milliseconds getNextIoDuration() { return nextIoDuration; }
  int getId() override { return id; }
  ~task() = default;
  task(task &&) = default;
  task(std::chrono::milliseconds cpu = std::chrono::milliseconds(10),
       std::chrono::milliseconds io = std::chrono::milliseconds(10))
      : cpuTime(cpu), ioTime(io), cpuTimeCompleted(0), ioTimeCompleted(0),
        nextIoDuration(0), priority(basics::runnablePriority::First) {}
};

#endif // OSLEARNINGS_TASK_HPP
