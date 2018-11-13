// EPOS Semaphore Component Test Program

#include <utility/ostream.h>
#include <utility/stringstream.h>
#include <utility/guard.h>
#include <thread.h>
#include <semaphore.h>
#include <alarm.h>
#include <display.h>

using namespace EPOS;

const int iterations = 2;
#define log(argument) db<Synchronizer>(WRN) << argument;

Guard table;
Guard guard;
OStream cout;

Thread * phil[5];
Semaphore * chopstick[5];
// bool chopstick[5];

void show_message(const char * message, int line, int column) {
    // Display::position( line, column );
    cout << message << "(" << line << ")" << endl;
}

void show_message(StringStream * message, int line, int column) {
    // Display::position( line, column );
    cout << message;
    delete message;
}

// void get_chopstick(Future<int>* ) {
// }

int philosopher(int philosopher_index, int line, int column)
{
    int first = (philosopher_index < 4)? philosopher_index : 0;
    int second = (philosopher_index < 4)? philosopher_index + 1 : 4;
    line = philosopher_index;
    for(int i = iterations; i > 0; i--)
    {
        table.submit( &show_message, "thinking", line, column );
        Delay thinking(2000000);

        chopstick[first]->p();    // get first chopstick
        chopstick[second]->p();   // get second chopstick

        StringStream* stream1 = new StringStream{100};
        *stream1 << "Philosopher=" << philosopher_index << " got      the first=" << first 
                << " and second=" << second << " chopstick\n";
        table.submit( &show_message, stream1, line, column );

        // Future<bool>* first_chopstick = new Future<bool>();
        // Future<bool>* second_chopstick = new Future<bool>();
        // guard.submit( &get_chopstick, first_chopstick, second_chopstick )

        table.submit( &show_message, " eating ", line, column );
        Delay eating(1000000);

        chopstick[first]->v();    // release first chopstick
        chopstick[second]->v();   // release second chopstick

        StringStream* stream2 = new StringStream{100};
        *stream2 << "Philosopher=" << philosopher_index << " released the first=" << first 
                << " and second=" << second << " chopstick\n";
        table.submit( &show_message, stream2, line, column );
        
        // guard.submit( &release_chopstick, first_chopstick, second_chopstick )
    }

    table.submit( &show_message, "  done  ", line, column );
    return iterations;
}

void setup_program()
{
    Display::clear();
    Display::position(0, 0);
    cout << "The Philosopher's Dinner:" << endl;

    for(int i = 0; i < 5; i++)
        // chopstick[i] = true;
        chopstick[i] = new Semaphore;

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