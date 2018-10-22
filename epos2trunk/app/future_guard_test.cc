// EPOS Synchronizer Component Test Program

#include <thread.h>
#include <machine.h>
#include <utility/guard.h>
#include <utility/future.h>
#include <semaphore.h>
#include <alarm.h>

using namespace EPOS;
static volatile int counter = 0;

Guard guard;
Semaphore display_lock;

// #define log(argument) display_lock.p(); db<Synchronizer>(WRN) << argument; display_lock.v();
#define log(argument) db<Synchronizer>(WRN) << argument;

void increment_counter(Future<int>* future) {
    Delay thinking(1000000);
    counter = counter + 1;
    log( "increment_counter (counter=" << counter << ")" << endl )
    future->resolve(counter);
}

int functionA() {
    log( "functionA ()" << endl )
    Future<int>* future = new Future<int>();

    guard.submit(&increment_counter, future);
    log( "functionA (result=" << future->get_value() << ")" << endl )    return 0;
}

int functionB() {
    log( "functionB ()" << endl )
    Future<int>* future = new Future<int>();

    guard.submit(&increment_counter, future);
    log( "functionB (result=" << future->get_value() << ")" << endl )
    return 0;
}

int main()
{
    log( endl << "Starting main application..." << endl )

    Thread* producer = new Thread(&functionA);
    Thread* consumer = new Thread(&functionB);

    consumer->join();
    producer->join();

    log( "Exiting main application..." << endl )
    return 0;
}
