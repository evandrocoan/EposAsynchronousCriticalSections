// EPOS Synchronizer Component Test Program
#define DEBUG_SYNC

#include <thread.h>
#include <machine.h>
#include <utility/guard.h>
#include <alarm.h>

using namespace EPOS;

static volatile int counter = 0;
static const int iterations = 1e6;

Guard counter_guard;
Guard display_guard;
Thread * pool[5];

void show(char arg, const char * type) {
    LOG( arg << ": " << type << " (counter=" << counter << ")" << endl )
}

void increment_counter() {
    counter = counter + 1;
    if ((counter % 100000) == 0)
        LOG("increment_counter (counter=" << counter << ")" << endl )
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
    LOG( endl )
    LOG( "main: begin (counter=" << counter << ")" << endl )

    pool[0] = new Thread(&mythread, 'A');
    pool[1] = new Thread(&mythread, 'B');
    // pool[2] = new Thread(&mythread, 'C');
    // pool[3] = new Thread(&mythread, 'D');
    // pool[4] = new Thread(&mythread, 'E');

    LOG( "main: joining all threads (counter=" << counter << ")" << endl )
    pool[0]->join();
    pool[1]->join();
    // pool[2]->join();
    // pool[3]->join();
    // pool[4]->join();

    LOG( "main: done with all threads (counter=" << counter << ")" << endl )
    delete pool[0];
    delete pool[1];
    // delete pool[2];
    // delete pool[3];
    // delete pool[4];

    LOG( "main: exiting (counter=" << counter << ")" << endl )
    return 0;
}
