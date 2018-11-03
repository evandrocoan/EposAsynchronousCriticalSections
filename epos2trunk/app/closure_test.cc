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

void show(char arg1, const char * arg2, bool arg3, bool arg4, int arg5) {
    log( "arg1=" << arg1 << ", arg2=" << arg2 << ", arg3=" << arg3 << ", arg4=" << arg4 << ", arg5=" << arg5 << endl )
}

int main()
{
    log( endl << "main: begin()" << endl )
    counter_guard.submit(&show, 'A', "Test 1", true, false, 10);

    log( "main: exiting()" << endl )
    return 0;
}
