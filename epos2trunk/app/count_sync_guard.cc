// EPOS Synchronizer Component Test Program

#include <thread.h>
#include <machine.h>
#include <utility/guard.h>
#include <alarm.h>

using namespace EPOS;

static volatile int counter = 0;
static const int iterations = 1e0;

Guard counter_guard;
Guard display_guard;
Thread * pool[5];

int show(char arg, const char * type) {
    db<Synchronizer>(WRN) << arg << ": " << type << " (counter=" << counter << ")" << endl;
    return 0;
}

int increment_counter() {
    counter = counter + 1;
    // db<Synchronizer>(WRN)   << "increment_counter (counter=" << counter << ")" << endl;
    return 0;
}

int mythread(char arg) {
    display_guard.submit(&show, arg, "begin");

    for (int i = iterations; i > 0 ; i--) {
        counter_guard.submit(&increment_counter);
    }

    display_guard.submit(&show, arg, "end");
    return 0;
}

int main()
{
    db<Synchronizer>(WRN)   << "main: begin (counter=" << counter << ")" << endl;

    pool[0] = new Thread(&mythread, 'A');
    pool[1] = new Thread(&mythread, 'B');
    pool[2] = new Thread(&mythread, 'C');
    pool[3] = new Thread(&mythread, 'D');
    pool[4] = new Thread(&mythread, 'D');

    db<Synchronizer>(WRN)   << "main: start joining the threads (counter=" << counter << ")" << endl;

    // join waits for the threads to finish
    for(int i = 0; i < 5; i++) {
        pool[i]->join();
    }

    db<Synchronizer>(WRN)   << "main: done with both (counter=" << counter << ")" << endl;

    for(int i = 0; i < 5; i++)
        delete pool[i];

    db<Synchronizer>(WRN)   << "main: exiting (counter=" << counter << ")" << endl;

    return 0;
}
