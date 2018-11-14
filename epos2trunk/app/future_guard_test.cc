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

    LOG( Debug, WRN, "increment_counter (counter=" << counter << ")" << endl )
    future->resolve(counter);
}

int functionA() {
    LOG( Debug, WRN, "functionA ()" << endl )
    Future<int>* future = new Future<int>();

    guard.submit(&increment_counter, future);
    auto value = future->get_value();

    LOG( Debug, WRN, "functionA (result=" << value << ")" << endl )
    return 0;
}

int functionB() {
    LOG( Debug, WRN, "functionB ()" << endl )
    Future<int>* future = new Future<int>();

    guard.submit(&increment_counter, future);
    auto value = future->get_value();

    LOG( Debug, WRN, "functionB (result=" << value << ")" << endl )
    return 0;
}

int main()
{
    LOG( Debug, WRN, endl )
    LOG( Debug, WRN, "Starting main application..." << endl )

    Thread* producer = new Thread(&functionA);
    Thread* consumer = new Thread(&functionB);

    consumer->join();
    producer->join();

    LOG( Debug, WRN, "Exiting main application..." << endl )
    return 0;
}
