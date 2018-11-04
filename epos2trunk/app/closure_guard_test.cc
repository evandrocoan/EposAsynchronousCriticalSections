// EPOS Synchronizer Component Test Program

#include <thread.h>
#include <machine.h>
#include <utility/guard.h>
#include <alarm.h>

using namespace EPOS;

// #include <semaphore.h>
// Semaphore display_lock;
// #define log(argument) display_lock.p(); db<Synchronizer>(WRN) << argument; display_lock.v();
#define log(argument) db<Synchronizer>(WRN) << argument;

Guard counter_guard;

void show(char char1, const char* text1, const char* text2, bool bool1, bool bool2, const char* text3, int int1) {
    log( "char1=" << char1 << ", text1=" << text1 \
    << ", text2=" << text2 << ", bool1=" << bool1 << ", bool2=" << bool2 \
    << ", text3=" << text3 << ", int1="  << int1  << endl )
}

int main()
{
    log( endl << "main: begin()" << endl )
    counter_guard.submit(&show, 'A', "Test 1", "Test 2", true, false, "Test 3", 10);
    counter_guard.submit(&show, 'B', "Test 4", "Test 5", false, false, "Test 6", 20);

    log( "main: exiting()" << endl )
    return 0;
}
