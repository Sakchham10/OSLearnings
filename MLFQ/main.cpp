#include "linkedlist.hpp"
#include "queue.hpp"
#include "threadManager.hpp"

#include <iostream>

int main() {
    basics::queue<int> readyQueue;
    readyQueue.append(1);

    basics::linkedlist<int> completedJobs;
    completedJobs.append(readyQueue.popLeft());

    basics::threadManager<void()> threadManager(1);

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
