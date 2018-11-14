// EPOS Synchronizer Component Test Program
#define DEBUG_SYNC

#include <utility/ostream.h>
#include <utility/debug_sync.h>
#include <semaphore.h>
#include <thread.h>
#include <machine.h>

using namespace EPOS;
int counter = 0;

Semaphore counter_lock; 

// mythread()
// Simply adds 1 to counter repeatedly, in a loop
// No, this is not how you would add 10,000,000 to
// a counter, but it shows the problem nicely.
int mythread(char arg) {
    DB( arg << ": begin " << counter << endl )

    for (int i = 0; i < 1e4; i++) {
        counter_lock.p();
        counter = counter + 1;
        // DB( "counter=" << counter << endl )
        counter_lock.v();
    }

    DB( arg << ": done " << counter << endl )
    return 0;
}

// main()
// Just launches two threads (pthread_create)
// and then waits for them (pthread_join)
int main()
{
    DB( endl )
    DB( "main: begin (counter = " << counter << ")" << endl )
    Thread * p1 = new Thread(&mythread, 'A');
    Thread * p2 = new Thread(&mythread, 'B');
    Thread * p3 = new Thread(&mythread, 'C');
    Thread * p4 = new Thread(&mythread, 'D');
    Thread * p5 = new Thread(&mythread, 'E');

    // join waits for the threads to finish
    p1->join();
    p2->join();
    p3->join();
    p4->join();
    p5->join();
    DB( "main: done with both (counter = " << counter << ")" << endl )

    delete p1;
    delete p2;
    delete p3;
    delete p4;
    delete p5;

    return 0;
}
