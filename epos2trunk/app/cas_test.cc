// EPOS CAS Component Test Program

#include <utility/ostream.h>
#include <architecture/ia32/cpu.h>

using namespace EPOS;
OStream cout;

int main()
{
    cout << endl << "Welcome to the CPU::cas() instruction test!" << endl;
    int original = 5;
    int compare = 5;
    int replacement = 6;
    int replaced;

    cout << "original=" << original
            << ", compare=" << compare
            << ", replacement=" << replacement
            << ", replaced=" << replaced
            << endl;

    replaced = CPU::cas(original, compare, replacement);

    cout << "original=" << original
            << ", compare=" << compare
            << ", replacement=" << replacement
            << ", replaced=" << replaced
            << endl;

    cout << "The CPU::cas() instruction set ran successfully!" << endl << endl;
}
