// EPOS Synchronizer Component Test Program

#include <utility/ostream.h>
#include <semaphore.h>
#include <thread.h>

using namespace EPOS;

static volatile int counter = 0;

Semaphore mutex;

OStream cout;

// mythread()
// Simply adds 1 to counter repeatedly, in a loop
// No, this is not how you would add 10,000,000 to
// a counter, but it shows the problem nicely.
//
int mythread(char arg) {
    mutex.p();
    cout << arg << ": begin" << endl;
    mutex.v();
    int i;
    for (i = 0; i < 1e7; i++) {
        counter = counter + 1;
    }

    mutex.p();
    cout << arg << ": done" << endl;
    mutex.v();
    return 0;
}

// main()
// Just launches two threads (pthread_create)
// and then waits for them (pthread_join)
int main()
{
    Thread p1(&mythread, 'A');
    Thread p2(&mythread, 'B');

    mutex.p();
    cout << "main: begin (counter = " << counter << ")" << endl;
    mutex.v();

    // join waits for the threads to finish
    p1.join();
    p2.join();

    mutex.p();
    cout << "main: done with both (counter = " << counter << ")"<< endl;
    mutex.v();

    return 0;
}
