// Scheduler Test Program
// #define DEBUG_SYNC

#include <thread>
#include <sstream>
#include <sched.h>

#include "guard.h"
#define CONSOLE_MODE

const int iterations = 10;

Guard table;
std::thread* philosophers[5];
Future<int>* philosophers_result[5];

bool is_chopstick_free[5];
Future<int>* locked_futures[5];

void philosopher(int n, int l, int c, Future<int>* r);
void think(unsigned long long n);
void eat(unsigned long long n);
unsigned long long busy_wait(unsigned long long n);

#ifdef CONSOLE_MODE
    #define EXTRA(arg) arg
#else
    #define EXTRA(arg)
#endif

void show_message(const char * message, int line, int column) {
#ifdef CONSOLE_MODE
    DB( message << "(" << line << ")" << std::endl )
#else
    Display::position( line, column );
    LOG( message )
#endif
}

void show_message(std::stringstream * message, int line, int column) {
#ifdef CONSOLE_MODE
    DB( message )
#else
    Display::position( line, column );
    LOG( message )
#endif
    delete message;
}

void setup_program()
{
    Display::clear();
    Display::position(0, 0);
    LOG( "The Philosopher's Dinner:" << std::endl )

    for(int i = 0; i < 5; i++)
    {
        is_chopstick_free[i] = true;
        philosophers_result[i] = new Future<int>();
        locked_futures[i] = nullptr;
    }

    philosophers[0] = new std::thread(&philosopher, 0,  5, 30, philosophers_result[0]);
    philosophers[1] = new std::thread(&philosopher, 1, 10, 44, philosophers_result[1]);
    philosophers[2] = new std::thread(&philosopher, 2, 16, 39, philosophers_result[2]);
    philosophers[3] = new std::thread(&philosopher, 3, 16, 21, philosophers_result[3]);
    philosophers[4] = new std::thread(&philosopher, 4, 10, 17, philosophers_result[4]);

    LOG( "Philosophers are alive and hungry!" << std::endl )

    Display::position(7, 44);
    LOG( '/' )
    Display::position(13, 44);
    LOG( '\\' )
    Display::position(16, 35);
    LOG( '|' )
    Display::position(13, 27);
    LOG( '/' )
    Display::position(7, 27);
    LOG( '\\' )
    Display::position(19, 0);

    LOG( "The dinner is served ..." << std::endl )
}

// g++ -o test simple_guard_test.cpp -lpthread -m32  -ggdb && ./test
// /\) => (\dx[a-f\d]{8,8})/ /Philosopher=\d/ \bGuard::vouch\b \bGuard::clear\b \bchopstick=2\b
int main()
{
    table.submit( &setup_program );

    for(int i = 0; i < 5; i++) {
        philosophers[i]->join();
        int ret = philosophers_result[i]->get_value();
        std::stringstream* stream = new std::stringstream();

        *stream << "Philosopher " << i << " ate " << ret << " times\n";
        table.submit( &show_message, stream, 20 + i, 0 );
    }

    for(int i = 0; i < 5; i++) {
        delete philosophers[i];
        delete philosophers_result[i];
    }

    LOG( "The end!" << std::endl )
    return 0;
}

void release_chopstick(int philosopher_index, int chopstick_index,
        Future<int>* future_chopstick, const char* which_chopstick)
{
    DB( "Philosopher=" << philosopher_index
            << ", release chopstick=" << chopstick_index
            << ", is_chopstick_free=" << is_chopstick_free[chopstick_index]
            << ", future_chopstick=" << future_chopstick
            << ", locked_futures=" << locked_futures[chopstick_index]
            << ", " << which_chopstick )

    is_chopstick_free[chopstick_index] = true;
    delete future_chopstick;

    if( locked_futures[chopstick_index] ) {
        DB( std::endl )

        auto old = locked_futures[chopstick_index];
        locked_futures[chopstick_index] = nullptr;
        old->resolve(1);
    }
    else {
        assert( locked_futures[chopstick_index] == nullptr );
        DB( std::endl )
    }
}

void get_chopstick(int philosopher_index, int chopstick_index,
        Future<int>* future_chopstick, const char* which_chopstick)
{
    DB( "Philosopher=" << philosopher_index
            << ", getting chopstick=" << chopstick_index
            << ", is_chopstick_free=" << is_chopstick_free[chopstick_index]
            << ", " << which_chopstick )

    if( is_chopstick_free[chopstick_index] ) {
        is_chopstick_free[chopstick_index] = false;

        DB( ", future_chopstick=" << future_chopstick << std::endl )
        future_chopstick->resolve(1);
    }
    else {
        assert( locked_futures[chopstick_index] == nullptr );
        locked_futures[chopstick_index] = future_chopstick;

        DB( ", locked_futures="
                << locked_futures[chopstick_index] << std::endl )
    }
}

void philosopher(int philosopher_index, int line, int column, Future<int>* result)
{
    int first = (philosopher_index < 4)? philosopher_index : 0;
    int second = (philosopher_index < 4)? philosopher_index + 1 : 4;

#ifdef CONSOLE_MODE
    line = philosopher_index;
#endif

    for(int i = iterations; i > 0; i--) {
        std::stringstream* stream1 = new std::stringstream();
        *stream1 << "thinking[" << sched_getcpu() << "]"
                EXTRA( << " (" << line << ")" << "\n" );
        table.submit( &show_message, stream1, line, column );
        think(1000000);

        std::stringstream* stream2 = new std::stringstream();
        *stream2 << "  hungry[" << sched_getcpu() << "]"
                EXTRA( << " (" << line << ")" << "\n" );
        table.submit( &show_message, stream2, line, column );

        // Get the first chopstick
        Future<int>* chopstick1 = new Future<int>();
        table.submit( &get_chopstick, philosopher_index, first, chopstick1, "FIRST " );
        chopstick1->get_value();

        // Get the second chopstick
        Future<int>* chopstick2 = new Future<int>();
        table.submit( &get_chopstick, philosopher_index, second, chopstick2, "SECOND" );
        chopstick2->get_value();

    #ifdef CONSOLE_MODE
        std::stringstream* stream6 = new std::stringstream();
        *stream6 << "Philosopher=" << philosopher_index
                << ", got     the first=" << first
                << " and second=" << second << " chopstick" << "\n";
        table.submit( &show_message, stream6, line, column );
    #endif

        std::stringstream* stream3 = new std::stringstream();
        *stream3 << " eating[" << sched_getcpu() << "] " EXTRA( << "\n" );
        table.submit( &show_message, stream3, line, column );

        eat(500000);

        std::stringstream* stream4 = new std::stringstream();
        *stream4 << "    ate[" << sched_getcpu() << "]"
                EXTRA( << " (" << line << ")" << "\n" );
        table.submit( &show_message, stream4, line, column );

        // Release the chopsticks
        table.submit( &release_chopstick, philosopher_index, first, chopstick1, "FIRST " );
        table.submit( &release_chopstick, philosopher_index, second, chopstick2, "SECOND" );

    #ifdef CONSOLE_MODE
        std::stringstream* stream7 = new std::stringstream();
        *stream7 << "Philosopher=" << philosopher_index
                << ", release the first=" << first
                << " and second=" << second << " chopstick" << "\n";
        table.submit( &show_message, stream7, line, column );
    #endif
    }

    std::stringstream* stream5 = new std::stringstream();
    *stream5 << "  done[" << sched_getcpu() << "]  "
            EXTRA( << " (" << line << ")" << "\n" );
    table.submit( &show_message, stream5, line, column );

    result->resolve(iterations);
}

void eat(unsigned long long n) {
    static unsigned long long v;
    v = busy_wait(n);
}

void think(unsigned long long n) {
    static unsigned long long v;
    v = busy_wait(n);
}

unsigned long long busy_wait(unsigned long long n)
{
    volatile unsigned long long v;
    for(long long int j = 0; j < 20 * n; j++)
        v &= 2 ^ j;
    return v;
}
