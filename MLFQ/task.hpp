//
// Created by Sakchham Sangroula on 7/30/26.
//

#ifndef OSLEARNINGS_TASK_HPP
#define OSLEARNINGS_TASK_HPP
#include "runnable.hpp"
#include <chrono>
#include <cstddef>
#include <random>
#include <ratio>

class task {
  int id;
  std::chrono::milliseconds cpuTime;
  std::chrono::milliseconds ioTime;
  std::chrono::milliseconds cpuTimeCompleted;
  std::chrono::milliseconds ioTimeCompleted;
  std::chrono::milliseconds nextIoDuration;
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
  bool shouldRunIO;

public:
  // TODO: make move constructor, see mlfq.hpp (std::move(task))
  void runTask(std::chrono::milliseconds quanta) {
    shouldRunIO = false;
    auto start = std::chrono::high_resolution_clock::now();
    while (true && cpuTimeCompleted < cpuTime) {
      auto end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double, std::milli> elapsed = end - start;
      if (elapsed >= quanta) {
        shouldRunIO = true;
        return;
      }
      auto work = getWork();
      if (work == 0) {
        std::chrono::milliseconds duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - end);
        cpuTimeCompleted += duration;
      } else {
        shouldRunIO = true;
        calculateNextIoDuration();
        std::chrono::milliseconds duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - end);
        cpuTimeCompleted += duration;
        return;
      }
    }
    if (ioTime > ioTimeCompleted) {
      calculateNextIoDuration();
      return;
    }
    return;
  }
  int getId() { return id; }
  std::chrono::milliseconds getIO() { return nextIoDuration; }
  bool isIO() { return shouldRunIO; };
  ~task() = default;
  task(task &&) = default;
  task(std::chrono::milliseconds cpu = std::chrono::milliseconds(10),
       std::chrono::milliseconds io = std::chrono::milliseconds(10))
      : cpuTime(cpu), ioTime(io), cpuTimeCompleted(0), ioTimeCompleted(0),
        nextIoDuration(0) {}
};

#endif // OSLEARNINGS_TASK_HPP
