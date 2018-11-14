// EPOS Synchronizer Component Test Program
#define DEBUG_SYNC

#include <thread.h>
#include <utility/future.h>
#include <semaphore.h>
#include <alarm.h>

using namespace EPOS;

int producerFunction(Future<int>* future) {
    DB( "producerFunction ()" << endl )
    Delay thinking(1000000);
    future->resolve(10);

    DB( "producerFunction (resolving future=" << future << " to 10)" << endl )
    return 0;
}

int consumerFunction(Future<int>* future) {
    DB( "consumerFunction ()" << endl )

    auto value = future->get_value();
    DB( "consumerFunction (result=" << value << ")" << endl )

    value = future->get_value();
    DB( "consumerFunction (result=" << value << ")" << endl )
    return 0;
}

int main()
{
    DB( endl )
    DB( "Starting main application..." << endl )
    Future<int>* future = new Future<int>();

    Thread* consumer = new Thread(&consumerFunction, future);
    Thread* producer = new Thread(&producerFunction, future);

    consumer->join();
    producer->join();

    DB( "Exiting main application..." << endl )
    return 0;
}
