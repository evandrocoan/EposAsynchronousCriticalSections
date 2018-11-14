// EPOS Synchronizer Component Test Program
#define DEBUG_SYNC

#include <semaphore.h>
#include <utility/debug_sync.h>
#include <thread.h>
#include <machine.h>
#include <alarm.h>

using namespace EPOS;
int counter = 0;

// mythread()
// Simply adds 1 to counter repeatedly, in a loop
// No, this is not how you would add 10,000,000 to
// a counter, but it shows the problem nicely.
int mythread(char arg) {
    DB( arg << ": begin : " << endl)

    for (int i = 0; i < 1e6; i++) {
        if (counter%100000 == 0){
            DB( arg << " : " << counter << endl )
        }
        counter = counter + 1;
        // DB( "Counting " << counter << endl )
    }

    DB( arg << ": done" << endl )
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
    Thread * p6 = new Thread(&mythread, 'F');

    // join waits for the threads to finish
    p1->join();
    p2->join();
    p3->join();
    p4->join();
    p5->join();
    p6->join();

    DB( "main: done with both (counter = " << counter << ")"<< endl )
    return 0;
}
