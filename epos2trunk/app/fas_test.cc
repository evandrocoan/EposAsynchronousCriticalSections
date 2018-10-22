// EPOS Synchronizer Component Test Program

#include <utility/ostream.h>
#include <semaphore.h>
#include <thread.h>
#include <machine.h>
#include <cpu.h>

using namespace EPOS;

Semaphore display_lock;
// #define log(argument) display_lock.p(); db<Synchronizer>(WRN) << argument; display_lock.v();
#define log(argument) db<Synchronizer>(WRN) << argument;

static const int iterations = 1e5;

int old = 0;
int current = 10;
int next = 11;

#define check(thread, name) \
    log( thread << name \
            << ", old=" << old \
            << ", current=" << current \
            << ", next=" << next \
            << endl )

int myThread1() {
    check("Thread 1", ", begin")
    
    for (int i = 0; i < iterations; i++) 
    {
        old = CPU::cas(current, current, next);
        current = CPU::cas(next, next, old);
        next = CPU::cas(old, old, current);
        // check("Thread 1", ", now")
    }

    check("Thread 1", ", result")
    return 0;
}

int myThread2() {
    check("Thread 2", ", begin")

    for (int i = 0; i < iterations; i++) {
        old = CPU::cas(current, current, next);
        current = CPU::cas(next, next, old);
        next = CPU::cas(old, old, current);
        // check("Thread 2", ", now")
    }

    check("Thread 2", ", result")
    return 0;
}

int main()
{
    log( "iterations=" << iterations << endl )
    Thread p1(&myThread1);
    Thread p2(&myThread2);

    check("Thread 0", ", main")
    p1.join();
    p2.join();

    check("Thread 0", ", end")
    return 0;
}