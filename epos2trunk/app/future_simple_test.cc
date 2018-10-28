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

int producerFunction(Future<int>* future) {
    Delay thinking(1000000);
    future->resolve(10);

    log( "producerFunction (resolving future=" << future << " to 10)" << endl )
    return 0;
}

int consumerFunction(Future<int>* future) {
    log( "consumerFunction ()" << endl )
    log( "consumerFunction (result=" << future->get_value() << ")" << endl )
    log( "consumerFunction (result=" << future->get_value() << ")" << endl )
    return 0;
}

int main()
{
    log( endl << "Starting main application..." << endl )
    Future<int>* future = new Future<int>();

    Thread* consumer = new Thread(&consumerFunction, future);
    Thread* producer = new Thread(&producerFunction, future);

    consumer->join();
    producer->join();

    log( "Exiting main application..." << endl )
    return 0;
}
