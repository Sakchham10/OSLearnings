//
// Created by Sakchham Sangroula on 7/30/26.
//

#ifndef OSLEARNINGS_TASK_HPP
#define OSLEARNINGS_TASK_HPP
#include <cstddef>
#include <functional>

class task{
    std::size_t priority;
    std::function<void()> taskFunc;
    public:
        void runTask(){
            taskFunc();
        }
        
};

#endif //OSLEARNINGS_TASK_HPP