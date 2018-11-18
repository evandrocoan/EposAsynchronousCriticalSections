
#define DEBUG_SYNC
#include <thread>
#include "guard.h"

int counter = 0;
static const int iterations = 1e3;

Guard counter_guard;
Guard display_guard;

void show(char arg, const char * type) {
    LOG( arg << ": " << type << " (counter=" << counter << ")" << std::endl )
}

void increment_counter() {
    counter = counter + 1;
    // DB( "increment_counter (counter=" << counter << ")" << std::endl )
}

int mythread(char arg) {
    display_guard.submit(&show, arg, "begin");

    for (int i = iterations; i > 0 ; i--) {
        counter_guard.submit(&increment_counter);
    }

    display_guard.submit(&show, arg, "end");
    return 0;
}

// g++ -o test simple_guard_test.cpp -lpthread -m32  -ggdb && ./test
// /\) => (\dx[a-f\d]{8,8})/ /Philosopher=\d/ \bGuard::vouch\b \bGuard::clear\b \bchopstick=2\b
int main() {
    LOG( std::endl )
    LOG( "main: begin (counter=" << counter << ")" << std::endl )

    std::thread thread1(mythread, 'A');
    std::thread thread2(mythread, 'B');
    std::thread thread3(mythread, 'C');
    std::thread thread4(mythread, 'D');
    std::thread thread5(mythread, 'E');

    LOG( "main: start joining the threads (counter=" << counter << ")" << std::endl )
    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();
    thread5.join();

    LOG( "main: exiting (counter=" << counter << ")" << std::endl )
    return 0;
}

