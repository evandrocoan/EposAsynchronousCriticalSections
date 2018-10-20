// EPOS Synchronizer Component Test Program

#include <utility/ostream.h>
#include <semaphore.h>
#include <thread.h>
#include <machine.h>
#include <cpu.h>

using namespace EPOS;

static volatile int shared_var = 0;

Semaphore display_lock;
OStream cout;
#define log(a) display_lock.p(); cout << a; display_lock.v();

int value = 10;
int replace = 11;

int myThread0() {
    log( Thread::self() << ": begin" << endl )
    int ret;

    for (int i = 0; i < 1e7; i++) {
        ret = CPU::fas(value, replace);
    }

    log( ret << ": done" << endl )
    return 0;
}

int myThread1() {
    log( Thread::self() << ": begin" << endl )
    int ret;

    for (int i = 0; i < 1e7; i++) {
        ret = CPU::fas(value, replace);
    }

    log( ret << ": done" << endl )
    return 0;
}

int main()
{
    Thread p1(&myThread0);
    Thread p2(&myThread1);

    log( "main: begin (value = " << value << ", replace = " << replace << ")" << endl )

    // join waits for the threads to finish
    p1.join();
    p2.join();

    log( "main: done with both (value = " << value << ", replace = " << replace << ")" << endl )

    return 0;
}