// EPOS Synchronizer Component Test Program

#include <utility/ostream.h>
#include <semaphore.h>
#include <thread.h>
#include <machine.h>
#include <cpu.h>

using namespace EPOS;

static volatile int shared_var = 0;

Semaphore display_lock;
Semaphore counter_lock; 

OStream cout;

int mythread0() {
    display_lock.p();
    cout << Thread0 << ": begin" << endl;
    display_lock.v();
    for (int i = 0; i < 1e7; i++) {
        int ret = CPU::fas(value, replace);
    }

    display_lock.p();
    cout << arg << ": done" << endl;
    display_lock.v();
    return 0;
}

int mythread1() {
    display_lock.p();
    cout << Thread1 << ": begin" << endl;
    display_lock.v();
    for (int i = 0; i < 1e7; i++) {
        int ret = CPU::fas(value, replace);
    }

    display_lock.p();
    cout << arg << ": done" << endl;
    display_lock.v();
    return 0;
}

int main()
{
    Thread p1(&mythread, 'A');
    Thread p2(&mythread, 'B');

    display_lock.p();
    cout << "main: begin (counter = " << counter << ")" << endl;
    display_lock.v();

    // join waits for the threads to finish
    p1.join();
    p2.join();

    cout << "main: done with both (counter = " << counter << ")"<< endl;

    return 0;
}