#include "../h/scheduler.hpp"

// definicija statickog clana (jedan jedini red spremnih niti u sistemu)
List<_thread> Scheduler::redSpremnih;

_thread *Scheduler::get()
{
    return redSpremnih.removeFirst();
}

void Scheduler::put(_thread *t)
{
    redSpremnih.addLast(t);
}
