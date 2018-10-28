// EPOS Synchronizer Component Test Program

#include <thread.h>
#include <machine.h>
#include <utility/guard.h>
#include <utility/future.h>
#include <semaphore.h>
#include <alarm.h>

using namespace EPOS;

Guard guard;
Semaphore display_lock;

// #define log(argument) display_lock.p(); db<Synchronizer>(WRN) << argument; display_lock.v();
#define log(argument) db<Synchronizer>(WRN) << argument;

int producerFunction(EPOS::S::U::Future<int>* future) {
    Delay thinking(1000000);
    future->resolve(10);

    log( "producerFunction (resolving future=" << future << " to 10)" << endl )
    return 0;
}

int consumerFunction() {
    log( "consumerFunction ()" << endl )
    EPOS::S::U::Future<int>* future = new EPOS::S::U::Future<int>();

    Thread* producer = new Thread(&producerFunction, future);
    log( "consumerFunction (result=" << future->get_value() << ")" << endl )
    log( "consumerFunction (result=" << future->get_value() << ")" << endl )

    producer->join();
    return 0;
}

int main()
{
    log( endl << "Starting main application..." << endl )
    Thread* consumer = new Thread(&consumerFunction);
    consumer->join();

    log( "Exiting main application..." << endl )
    return 0;
}
