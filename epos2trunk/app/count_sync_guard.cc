// EPOS Synchronizer Component Test Program

#include <thread.h>
#include <machine.h>
#include <utility/guard.h>
#include <alarm.h>

using namespace EPOS;

static volatile int counter = 0;
static const int iterations = 1e3;

// #include <semaphore.h>
// Semaphore display_lock;
// #define log(argument) display_lock.p(); db<Synchronizer>(WRN) << argument; display_lock.v();
#define log(argument) db<Synchronizer>(WRN) << argument;

Guard counter_guard;
Guard display_guard;
Thread * pool[5];

void show(char arg, const char * type) {
    log( arg << ": " << type << " (counter=" << counter << ")" << endl )
}

void increment_counter() {
    counter = counter + 1;
    // log( "increment_counter (counter=" << counter << ")" << endl )
}

int mythread(char arg) {
    display_guard.submit(&show, arg, "begin");

    for (int i = iterations; i > 0 ; i--) {
        counter_guard.submit(&increment_counter);
        // Delay label(1000);
    }

    display_guard.submit(&show, arg, "end");
    return 0;
}

int main()
{
    log( endl << "main: begin (counter=" << counter << ")" << endl )

    pool[0] = new Thread(&mythread, 'A');
    pool[1] = new Thread(&mythread, 'B');
    pool[2] = new Thread(&mythread, 'C');
    pool[3] = new Thread(&mythread, 'D');
    pool[4] = new Thread(&mythread, 'E');

    log( "main: start joining the threads (counter=" << counter << ")" << endl )
    for(int i = 0; i < 5; i++) {
        pool[i]->join();
    }

    log( "main: done with both (counter=" << counter << ")" << endl )
    for(int i = 0; i < 5; i++)
        delete pool[i];

    log( "main: exiting (counter=" << counter << ")" << endl )
    return 0;
}
