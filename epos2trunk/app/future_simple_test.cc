// EPOS Synchronizer Component Test Program
#define DEBUG_SYNC

#include <thread.h>
#include <utility/future.h>
#include <semaphore.h>
#include <alarm.h>

using namespace EPOS;

int producerFunction(Future<int>* future) {
    LOG( Debug, WRN, "producerFunction ()" << endl )
    Delay thinking(1000000);
    future->resolve(10);

    LOG( Debug, WRN, "producerFunction (resolving future=" << future << " to 10)" << endl )
    return 0;
}

int consumerFunction(Future<int>* future) {
    LOG( Debug, WRN, "consumerFunction ()" << endl )

    auto value = future->get_value();
    LOG( Debug, WRN, "consumerFunction (result=" << value << ")" << endl )

    value = future->get_value();
    LOG( Debug, WRN, "consumerFunction (result=" << value << ")" << endl )
    return 0;
}

int main()
{
    LOG( Debug, WRN, endl )
    LOG( Debug, WRN, "Starting main application..." << endl )
    Future<int>* future = new Future<int>();

    Thread* consumer = new Thread(&consumerFunction, future);
    Thread* producer = new Thread(&producerFunction, future);

    consumer->join();
    producer->join();

    LOG( Debug, WRN, "Exiting main application..." << endl )
    return 0;
}
