#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "../h/list.hpp"

class _thread;

class Scheduler{
public:
    static void put(_thread* t);
    static _thread* get();
private:
    static List<_thread> redSpremnih;
};

#endif