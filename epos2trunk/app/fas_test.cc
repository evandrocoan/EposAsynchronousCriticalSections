// EPOS Synchronizer Component Test Program
#define DEBUG_SYNC

#include <utility/ostream.h>
#include <utility/debug_sync.h>
#include <semaphore.h>
#include <thread.h>
#include <machine.h>
#include <cpu.h>

using namespace EPOS;
static const int iterations = 1e5;

int old = 0;
int current = 10;
int next = 11;

#define check(thread, name) \
    LOG( thread << name \
            << ", old=" << old \
            << ", current=" << current \
            << ", next=" << next \
            << endl )

int myThread1() {
    check("Thread 1", ", begin")

    for (int i = 0; i < iterations; i++)
    {
        old = CPU::fas(current, next);
        current = CPU::fas(next, old);
        next = CPU::fas(old, current);
        // check("Thread 1", ", now")
    }

    check("Thread 1", ", result")
    return 0;
}

int myThread2() {
    check("Thread 2", ", begin")

    for (int i = 0; i < iterations; i++) {
        old = CPU::fas(current, next);
        current = CPU::fas(next, old);
        next = CPU::fas(old, current);
        // check("Thread 2", ", now")
    }

    check("Thread 2", ", result")
    return 0;
}

int main()
{
    LOG( endl )
    LOG( "iterations=" << iterations << endl )
    Thread p1(&myThread1);
    Thread p2(&myThread2);

    check("Thread 0", ", main")
    p1.join();
    p2.join();

    check("Thread 0", ", end")
    return 0;
}
