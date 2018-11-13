// EPOS Semaphore Component Test Program

#include <utility/stringstream.h>

using namespace EPOS;

#define log(argument) db<Synchronizer>(WRN) << argument;

int main()
{
    log( endl << "Welcome to the `stringstream.h` test program" << endl )
    StringStream stream98chars{100};
    StringStream stream99chars{100};
    StringStream stream100chars{100};
    StringStream stream101chars{100};

    stream98chars  << "66666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666660098";
    stream99chars  << "666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666600099";
    stream100chars << "6666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666000100";
    stream101chars << "66666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666660000101";

    log( &stream98chars << endl )
    log( &stream99chars << endl )
    log( &stream100chars << endl )
    log( &stream101chars << endl << endl )

    StringStream base48chars{100};
    StringStream stream98chars2{100};
    StringStream stream99chars2{100};
    StringStream stream100chars2{100};
    StringStream stream101chars2{100};

    base48chars     << "777777777777777777777777777777777777777777777777";
    stream98chars2  << base48chars << "88888888888888888888888888888888888888888888880098";
    stream99chars2  << base48chars << "888888888888888888888888888888888888888888888800099";
    stream100chars2 << base48chars << "8888888888888888888888888888888888888888888888000100";
    stream101chars2 << base48chars << "88888888888888888888888888888888888888888888880000101";

    log( &stream98chars2 << endl )
    log( &stream99chars2 << endl )
    log( &stream100chars2 << endl )
    log( &stream101chars2 << endl )

    return 0;
}