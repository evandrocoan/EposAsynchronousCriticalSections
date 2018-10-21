// EPOS Synchronizer Component Test Program

#include <semaphore.h>
#include <thread.h>
#include <machine.h>
#include <alarm.h>

using namespace EPOS;

static volatile int counter = 0;

Semaphore display_lock;
Semaphore counter_lock; 

// mythread()
// Simply adds 1 to counter repeatedly, in a loop
// No, this is not how you would add 10,000,000 to
// a counter, but it shows the problem nicely.
int mythread(char arg) {
    display_lock.p();
    cout << arg << ": begin : " << endl;
    display_lock.v();

for (int i = 0; i < 1e7; i++) {
        if (counter%100000 == 0){
           // display_lock.p();
            cout << arg << " : " << counter << endl;
           // display_lock.v();
        }
        counter = counter + 1;
        db<Synchronizer>(WRN)   << "Counting " << counter
                                << endl;
    }

    display_lock.p();
    cout << arg << ": done" << endl;
    display_lock.v();
    return 0;
}

// main()
// Just launches two threads (pthread_create)
// and then waits for them (pthread_join)
int main()
{
    display_lock.p();
    cout << "main: begin (counter = " << counter << ")" << endl;
    display_lock.v();

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
    cout << "main: done with both (counter = " << counter << ")"<< endl;

    return 0;
}
