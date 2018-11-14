// EPOS Synchronizer Component Test Program
#define DEBUG_SYNC

#include <thread.h>
#include <utility/guard.h>
#include <utility/future.h>
#include <semaphore.h>
#include <alarm.h>

using namespace EPOS;
static volatile int counter = 0;

Guard guard;

void increment_counter(Future<int>* future) {
    Delay thinking(1000000);
    counter = counter + 1;

    DB( "increment_counter (counter=" << counter << ")" << endl )
    future->resolve(counter);
}

int functionA() {
    DB( "functionA ()" << endl )
    Future<int>* future = new Future<int>();

    guard.submit(&increment_counter, future);
    auto value = future->get_value();

    DB( "functionA (result=" << value << ")" << endl )
    return 0;
}

int functionB() {
    DB( "functionB ()" << endl )
    Future<int>* future = new Future<int>();

    guard.submit(&increment_counter, future);
    auto value = future->get_value();

    DB( "functionB (result=" << value << ")" << endl )
    return 0;
}

int main()
{
    DB( endl )
    DB( "Starting main application..." << endl )

    Thread* producer = new Thread(&functionA);
    Thread* consumer = new Thread(&functionB);

    consumer->join();
    producer->join();

    DB( "Exiting main application..." << endl )
    return 0;
}
