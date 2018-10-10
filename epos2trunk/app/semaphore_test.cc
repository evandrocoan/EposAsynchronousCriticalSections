// EPOS Semaphore Component Test Program

#include <utility/ostream.h>
#include <thread.h>
#include <mutex.h>
#include <guard.h>
#include <semaphore.h>
#include <alarm.h>
#include <display.h>
#include <cpu.h>

using namespace EPOS;

const int iterations = 2;

Mutex table;

Guard g;

Thread * phil[5];
Semaphore * chopstick[5];

OStream cout;

int philosopher(int n, int l, int c)
{
    int first = (n < 4)? n : 0;
    int second = (n < 4)? n + 1 : 4;

    for(int i = iterations; i > 0; i--) {

        table.lock();
        Display::position(l, c);
        cout << "thinking";
        table.unlock();

        Delay thinking(2000000);

        chopstick[first]->p();    // get first chopstick
        chopstick[second]->p();   // get second chopstick

        table.lock();
        Display::position(l, c);
        cout << " eating ";
        table.unlock();

        Delay eating(1000000);

        chopstick[first]->v();    // release first chopstick
        chopstick[second]->v();   // release second chopstick
    }

    table.lock();
    Display::position(l, c);
    cout << "  done  ";
    table.unlock();

    return iterations;
}

int main()
{
    cout << "The Philosopher's Dinner:" << endl;

    int a = 1;
    int b = 2;

    cout << "a : before = " << a << endl;
    cout << "b : before = " << b << endl;
    int b = CPU::fas(a, b);
    cout << "a : after = " << a << endl;

    cout << "The end!" << endl;

    return 0;
}
