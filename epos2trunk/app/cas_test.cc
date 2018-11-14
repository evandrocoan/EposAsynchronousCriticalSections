// EPOS CAS Component Test Program
#define DEBUG_SYNC

#include <utility/ostream.h>
#include <architecture/ia32/cpu.h>
#include <utility/debug_sync.h>

using namespace EPOS;
OStream cout;

int main()
{
    DB( endl )
    DB( "Welcome to the CPU::cas() instruction test!" << endl )
    int original = 5;
    int compare = 5;
    int replacement = 6;
    int replaced;

    DB( "original=" << original
            << ", compare=" << compare
            << ", replacement=" << replacement
            << ", replaced=" << replaced
            << endl )

    replaced = CPU::cas(original, compare, replacement);

    DB( "original=" << original
            << ", compare=" << compare
            << ", replacement=" << replacement
            << ", replaced=" << replaced
            << endl )

    DB( "The CPU::cas() instruction set ran successfully!" << endl )
}
