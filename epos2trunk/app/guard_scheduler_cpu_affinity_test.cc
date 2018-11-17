// EPOS Scheduler Test Program
// #define DEBUG_SYNC

#include <utility/ostream.h>
#include <utility/stringstream.h>
#include <utility/guard.h>
#include <utility/future.h>
#include <machine.h>
#include <display.h>
#include <thread.h>

using namespace EPOS;

#if !defined(nullptr)
    #define nullptr 0
#endif

#define CONSOLE_MODE
const int iterations = 10;
OStream cout;

Guard table;
Thread * phil[5];

bool is_chopstick_free[5];
Future<int>* locked_futures[5];

int philosopher(int n, int l, int c);
void think(unsigned long long n);
void eat(unsigned long long n);
unsigned long long busy_wait(unsigned long long n);

// Fix `error: undefined reference to endl` when compiling with hysterically_debugged = true
EPOS::S::U::OStream::Endl EPOS::S::U::endl;

#ifdef CONSOLE_MODE
    #define LVL WRN
#else
    #define LVL FFF
#endif

void show_message(const char * message, int line, int column) {
#ifdef CONSOLE_MODE
    DB( Synchronizer, LVL, message << "(" << line << ")" << endl )
#else
    Display::position( line, column );
    cout << message;
#endif
}

void show_message(StringStream * message, int line, int column) {
#ifdef CONSOLE_MODE
    DB( Synchronizer, LVL, message )
#else
    Display::position( line, column );
    cout << message;
#endif
    delete message;
}

void setup_program()
{
    Display::clear();
    Display::position(0, 0);
    cout << "The Philosopher's Dinner:" << endl;

    for(int i = 0; i < 5; i++)
    {
        is_chopstick_free[i] = true;
        locked_futures[i] = nullptr;
    }

    phil[0] = new Thread(&philosopher, 0,  5, 30);
    phil[1] = new Thread(&philosopher, 1, 10, 44);
    phil[2] = new Thread(&philosopher, 2, 16, 39);
    phil[3] = new Thread(&philosopher, 3, 16, 21);
    phil[4] = new Thread(&philosopher, 4, 10, 17);

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
        StringStream* stream = new StringStream{100};

        *stream << "Philosopher " << i << " ate " << ret << " times\n";
        table.submit( &show_message, stream, 20 + i, 0 );
    }

    for(int i = 0; i < 5; i++)
        delete phil[i];

    cout << "The end!" << endl;
    return 0;
}

void release_chopstick(int philosopher_index, int chopstick_index,
        Future<int>* future_chopstick, const char* which_chopstick)
{
    DB( Synchronizer, LVL, "Philosopher=" << philosopher_index
            << ", release chopstick=" << chopstick_index
            << ", is_chopstick_free=" << is_chopstick_free[chopstick_index]
            << ", future_chopstick=" << future_chopstick
            << ", locked_futures=" << locked_futures[chopstick_index]
            << ", " << which_chopstick )

    is_chopstick_free[chopstick_index] = true;
    delete future_chopstick;

    if( locked_futures[chopstick_index] ) {
        DB( Synchronizer, LVL, endl )

        auto old = locked_futures[chopstick_index];
        locked_futures[chopstick_index] = nullptr;
        old->resolve(1);
    }
    else {
        assert( locked_futures[chopstick_index] == nullptr );
        DB( Synchronizer, LVL, endl )
    }
}

void get_chopstick(int philosopher_index, int chopstick_index,
        Future<int>* future_chopstick, const char* which_chopstick)
{
    DB( Synchronizer, LVL, "Philosopher=" << philosopher_index
            << ", getting chopstick=" << chopstick_index
            << ", is_chopstick_free=" << is_chopstick_free[chopstick_index]
            << ", " << which_chopstick )

    if( is_chopstick_free[chopstick_index] ) {
        is_chopstick_free[chopstick_index] = false;

        DB( Synchronizer, LVL, ", future_chopstick=" << future_chopstick << endl )
        future_chopstick->resolve(1);
    }
    else {
        assert( locked_futures[chopstick_index] == nullptr );
        locked_futures[chopstick_index] = future_chopstick;

        DB( Synchronizer, LVL, ", locked_futures="
                << locked_futures[chopstick_index] << endl )
    }
}

int philosopher(int philosopher_index, int line, int column)
{
    int first = (philosopher_index < 4)? philosopher_index : 0;
    int second = (philosopher_index < 4)? philosopher_index + 1 : 4;

#ifdef CONSOLE_MODE
    line = philosopher_index;
#endif

    for(int i = iterations; i > 0; i--) {
        StringStream* stream1 = new StringStream{101};
        *stream1 << "thinking[" << Machine::cpu_id() << "]" << " (" << line << ")" << "\n";
        table.submit( &show_message, stream1, line, column );
        think(1000000);

        StringStream* stream2 = new StringStream{102};
        *stream2 << "  hungry[" << Machine::cpu_id() << "]" << " (" << line << ")" << "\n";
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
        StringStream* stream6 = new StringStream{103};
        *stream6 << "Philosopher=" << philosopher_index
                << ", got     the first=" << first
                << " and second=" << second << " chopstick" << "\n";
        table.submit( &show_message, stream6, line, column );
    #endif

        StringStream* stream3 = new StringStream{104};
        *stream3 << " eating[" << Machine::cpu_id() << "] " << "\n";
        table.submit( &show_message, stream3, line, column );

        eat(500000);

        StringStream* stream4 = new StringStream{105};
        *stream4 << "    ate[" << Machine::cpu_id() << "]" << " (" << line << ")" << "\n";
        table.submit( &show_message, stream4, line, column );

        // Release the chopsticks
        table.submit( &release_chopstick, philosopher_index, first, chopstick1, "FIRST " );
        table.submit( &release_chopstick, philosopher_index, second, chopstick2, "SECOND" );

    #ifdef CONSOLE_MODE
        StringStream* stream7 = new StringStream{106};
        *stream7 << "Philosopher=" << philosopher_index
                << ", release the first=" << first
                << " and second=" << second << " chopstick" << "\n";
        table.submit( &show_message, stream7, line, column );
    #endif
    }

    StringStream* stream5 = new StringStream{107};
    *stream5 << "  done[" << Machine::cpu_id() << "]  "
            << " (" << philosopher_index << ")" << "\n";
    table.submit( &show_message, stream5, philosopher_index, column );

    return iterations;
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
