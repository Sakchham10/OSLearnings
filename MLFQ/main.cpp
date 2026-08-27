#include "linkedlist.hpp"
#include "queue.hpp"
#include "threadManager.hpp"

int main() {
  basics::queue<int> readyQueue;
  readyQueue.append(1);

  basics::linkedlist<int> completedJobs;
  completedJobs.append(readyQueue.popLeft());

  basics::threadManager threadManager(10);

  return 0;
}