// EPOS Heap Utility Implementation

#include <utility/heap.h>
#include <utility/guard.h>

extern "C" { void _panic(); }

__BEGIN_UTIL

// Methods
void Simple_Heap::out_of_memory()
{

    Guard::pf_cs();

    db<Heaps>(ERR) << "Heap::alloc(this=" << this << "): out of memory!" << endl;

    _panic();
}

__END_UTIL
