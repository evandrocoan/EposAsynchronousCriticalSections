// EPOS Semaphore Component Test Program
#define DEBUG_SYNC

#include <utility/stringstream.h>
using namespace EPOS;

int main()
{
    DB( endl )
    DB( "Welcome to the `stringstream.h` test program" << endl )

    StringStream stream48chars{50};
    StringStream stream49chars{50};
    StringStream stream50chars{50};
    StringStream stream51chars{50};

    stream48chars << "666666666666666666666666666666666666666666660058";
    stream49chars << "6666666666666666666666666666666666666666666600049";
    stream50chars << "66666666666666666666666666666666666666666666000050";
    stream51chars << "666666666666666666666666666666666666666666660000051";

    DB( &stream48chars << endl )
    DB( &stream49chars << endl )
    DB( &stream50chars << endl )
    DB( &stream51chars << endl << endl )

    StringStream base23chars{50};
    StringStream stream48chars2{50};
    StringStream stream49chars2{50};
    StringStream stream50chars2{50};
    StringStream stream51chars2{50};

    base23chars    << "77777777777777777777777";
    stream48chars2 << base23chars << "8888888888888888888880048";
    stream49chars2 << base23chars << "88888888888888888888800049";
    stream50chars2 << base23chars << "888888888888888888888000050";
    stream51chars2 << base23chars << "8888888888888888888880000051";

    DB( &stream48chars2 << endl )
    DB( &stream49chars2 << endl )
    DB( &stream50chars2 << endl )
    DB( &stream51chars2 << endl )

    return 0;
}