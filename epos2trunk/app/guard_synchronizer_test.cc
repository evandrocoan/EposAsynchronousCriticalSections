// EPOS Synchronizer Component Test Program

#include <utility/ostream.h>
#include <utility/guard.h>
#include <utility/future.h>
#include <utility/debug_sync.h>
#include <utility/list.h>
#include <thread.h>
#include <alarm.h>

#if !defined(nullptr)
    #define nullptr 0
#endif

using namespace EPOS;

#define CONSOLE_MODE
const int iterations = 10;

Guard table;
OStream cout;

const int BUF_SIZE = 16;
char buffer[BUF_SIZE];

int full = 0;
int empty = BUF_SIZE;

Simple_List<Future<bool>*> pending_consume_futures;
Simple_List<Future<bool>*> pending_produce_futures;

#ifdef CONSOLE_MODE
    #define EXTRA(arg)
#else
    #define EXTRA
#endif

void consume() {
    EXTRA( LOG( "consume(empty=" << empty ) )

    Simple_List<Future<bool>*>::Element* element
            = pending_consume_futures.remove_tail();

    EXTRA( LOG( " element=" << element ) )
    EXTRA( LOG( ")" << endl ) )

    if( element )
    {
        Future<bool>* future = *element->object();
        future->resolve(true);
    }

    ++empty;
}

void can_consume(Future<bool>* can_consume) {
    EXTRA( LOG( "can_consume(full=" << full << ")" << endl ) )

    if(full > 0) {
        --full;
        can_consume->resolve(true);
    }
    else {
        Simple_List<Future<bool>*>::Element* element =
                new Simple_List<Future<bool>*>::Element(&can_consume);
        pending_consume_futures.insert(element);
    }
}

int consumer() {
    EXTRA( LOG( "consumer(this=" << Thread::self() << ")" << endl ) )

    int out = 0;
    for(int i = 0; i < iterations; i++)
    {
        // full.p();
        Future<bool>* future = new Future<bool>();
        table.submit(can_consume, future);
        future->get_value();

        cout << "C<-" << buffer[out] << "\t";
        out = (out + 1) % BUF_SIZE;
        Alarm::delay(5000);

        // empty.v();
        table.submit(consume);
    }

    return 0;
}

void can_produce(Future<bool>* can_consume) {
    EXTRA( LOG( "can_produce(empty=" << empty << ")" << endl ) )

    if(empty > 0) {
        --empty;
        can_consume->resolve(true);
    }
    else {
        Simple_List<Future<bool>*>::Element* element =
                new Simple_List<Future<bool>*>::Element(&can_consume);
        pending_produce_futures.insert(element);
    }
}

void produce() {
    EXTRA( LOG( "produce(full=" << full ) )

    Simple_List<Future<bool>*>::Element* element
            = pending_produce_futures.remove_tail();

    EXTRA( LOG( " element=" << element ) )
    EXTRA( LOG( ")" << endl ) )

    if( element )
    {
        Future<bool>* future = *element->object();
        future->resolve(true);
    }

    ++full;
}

int main() {
    Thread * cons = new Thread(&consumer);

    // producer
    int in = 0;
    for(int i = 0; i < iterations; i++) {
        // empty.p();
        Future<bool>* future = new Future<bool>();
        table.submit(can_produce, future);
        future->get_value();

        Alarm::delay(5000);
        buffer[in] = 'a' + in;
        cout << "P->" << buffer[in] << "\t";
        in = (in + 1) % BUF_SIZE;

        // full.v();
        table.submit(produce);
    }

    cons->join();
    cout << "The end!" << endl;

    delete cons;
    return 0;
}
