// EPOS Semaphore Component Test Program
#define DEBUG_SYNC

#include <utility/ostream.h>
#include <utility/stringstream.h>
#include <utility/guard.h>
#include <utility/future.h>
#include <thread.h>
#include <semaphore.h>
#include <alarm.h>
#include <display.h>

#if !defined(nullptr)
    #define nullptr 0
#endif

using namespace EPOS;

#define CONSOLE_MODE
const int iterations = 10;

Guard table;
Guard guard;

OStream cout;
Thread * phil[5];

bool is_chopstick_free[5];
Future<int>* locked_futures[5];

#ifdef CONSOLE_MODE
    #define LVL WRN
#else
    #define LVL FFF
#endif

void show_message(const char * message, int line, int column) {
#ifdef CONSOLE_MODE
    LOG( Synchronizer, LVL, message << "(" << line << ")" << endl )
#else
    Display::position( line, column );
    cout << message;
#endif
}

void show_message(StringStream * message, int line, int column) {
#ifdef CONSOLE_MODE
    LOG( Synchronizer, LVL, message )
#else
    Display::position( line, column );
    cout << message;
#endif
    delete message;
}

void release_chopsticks(int philosopher_index, int chopstick_index, Future<int>* future_chopstick, const char* which_chopstick) {
    LOG( Synchronizer, LVL, "Philosopher=" << philosopher_index
            << ", release chopstick=" << chopstick_index
            << ", is_chopstick_free=" << is_chopstick_free[chopstick_index]
            << ", future_chopstick=" << future_chopstick
            << ", locked_futures=" << locked_futures[chopstick_index]
            << ", " << which_chopstick )

    is_chopstick_free[chopstick_index] = true;
    delete future_chopstick;

    if( locked_futures[chopstick_index] ) {
        LOG( Synchronizer, LVL, endl )

        auto old = locked_futures[chopstick_index];
        locked_futures[chopstick_index] = nullptr;
        old->resolve(1);
    }
    else {
        assert( locked_futures[chopstick_index] == nullptr );
        LOG( Synchronizer, LVL, endl )
    }
}

void get_chopsticks(int philosopher_index, int chopstick_index, Future<int>* future_chopstick, const char* which_chopstick)
{
    LOG( Synchronizer, LVL, "Philosopher=" << philosopher_index
            << ", getting chopstick=" << chopstick_index
            << ", is_chopstick_free=" << is_chopstick_free[chopstick_index]
            << ", " << which_chopstick )

    if( is_chopstick_free[chopstick_index] ) {
        is_chopstick_free[chopstick_index] = false;

        LOG( Synchronizer, LVL, ", future_chopstick=" << future_chopstick << endl )
        future_chopstick->resolve(1);
    }
    else {
        assert( locked_futures[chopstick_index] == nullptr );
        locked_futures[chopstick_index] = future_chopstick;

        LOG( Synchronizer, LVL, ", locked_futures=" << locked_futures[chopstick_index] << endl )
    }
}

int philosopher(int philosopher_index, int line, int column)
{
    int first = (philosopher_index < 4)? philosopher_index : 0;
    int second = (philosopher_index < 4)? philosopher_index + 1 : 4;

#ifdef CONSOLE_MODE
    line = philosopher_index;
#endif

    for(int i = iterations; i > 0; i--)
    {
        table.submit( &show_message, "    thinking", line, column );
        Delay thinking(2000000);

        // Get the first chopstick
        Future<int>* chopstick1 = new Future<int>();
        guard.submit( &get_chopsticks, philosopher_index, first, chopstick1, "FIRST " );
        chopstick1->get_value(); 

        // Get the second chopstick
        Future<int>* chopstick2 = new Future<int>();
        guard.submit( &get_chopsticks, philosopher_index, second, chopstick2, "SECOND" );
        chopstick2->get_value(); 

    #ifdef CONSOLE_MODE
        StringStream* stream1 = new StringStream{100};
        *stream1 << "Philosopher=" << philosopher_index << ", got     the first=" << first
                << " and second=" << second << " chopstick\n";
        table.submit( &show_message, stream1, line, column );
    #endif

        table.submit( &show_message, "    eating  ", line, column );
        Delay eating(1000000);

        // Release the chopsticks
        guard.submit( &release_chopsticks, philosopher_index, second, chopstick2, "SECOND" );
        guard.submit( &release_chopsticks, philosopher_index, first, chopstick1, "FIRST " );

    #ifdef CONSOLE_MODE
        StringStream* stream2 = new StringStream{100};
        *stream2 << "Philosopher=" << philosopher_index << ", release the first=" << first
                << " and second=" << second << " chopstick\n";
        table.submit( &show_message, stream2, line, column );
    #endif
    }

    table.submit( &show_message, "    done    ", line, column );
    return iterations;
}

void setup_program()
{
    Display::clear();
    Display::position(0, 0);
    cout << "The Philosopher's Dinner:" << endl;

    for(int i = 0; i < 5; i++) {
        is_chopstick_free[i] = true;
        locked_futures[i] = nullptr;
    }
    phil[0] = new Thread(&philosopher, 0,  5, 32);
    phil[1] = new Thread(&philosopher, 1, 10, 44);
    phil[2] = new Thread(&philosopher, 2, 16, 39);
    phil[3] = new Thread(&philosopher, 3, 16, 24);
    phil[4] = new Thread(&philosopher, 4, 10, 20);

    cout << "Philosophers are alive and hungry!" << endl;

    Display::position(7, 44);
    cout << '/';
    Display::position(13, 44);
    cout << '\\';
    Display::position(16, 35);
    cout << '|';
    Display::position(13, 27);
    cout << '/';
    Display::position(7, 27);
    cout << '\\';
    Display::position(19, 0);

    cout << "The dinner is served ..." << endl;
}

int main()
{
    table.submit( &setup_program );

    for(int i = 0; i < 5; i++) {
        int ret = phil[i]->join();
        StringStream* stream = new StringStream(100);

        *stream << "Philosopher " << i << " ate " << ret << " times\n";
        table.submit( &show_message, stream, 20 + i, 0 );
    }

    for(int i = 0; i < 5; i++)
        delete phil[i];

    cout << "The end!" << endl;
    return 0;
}