// EPOS Synchronizer Component Test Program
#define DEBUG_SYNC

#include <utility/debug_sync.h>
#include <semaphore.h>
#include <thread.h>
#include <machine.h>

using namespace EPOS;
static volatile int counter = 0;

Semaphore counter_lock;

// mythread()
// Simply adds 1 to counter repeatedly, in a loop
// No, this is not how you would add 10,000,000 to
// a counter, but it shows the problem nicely.
int mythread(char arg) {
    LOG( arg << ": begin" << endl )
    for (int i = 0; i < 1e4; i++) {
        counter_lock.p();
        counter = counter + 1;
        counter_lock.v();
    }

    LOG( arg << ": done" << endl )
    return 0;
}

// main()
// Just launches two threads (pthread_create)
// and then waits for them (pthread_join)
int main()
{
    LOG( endl )
    LOG( "main: begin (counter = " << counter << ")" << endl )

    Thread p1(&mythread, 'A');
    Thread p2(&mythread, 'B');

    // join waits for the threads to finish
    p1.join();
    p2.join();

    LOG( "main: done with both (counter = " << counter << ")"<< endl )
    return 0;
}
