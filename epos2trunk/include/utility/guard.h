// EPOS Guard Component Declarations

#ifndef __guard_h
#define __guard_h

#include <architecture/ia32/cpu.h>
#include <utility/critical_section.h>
#include <utility/debug_sync.h>

__BEGIN_UTIL

class Guard
{
private:
    static const int NULL = 0;
    static const int DONE = 1;

public:
    Guard();
    ~Guard();

    template<typename ... Tn>
    void submit( void(*entry)( Tn ... ), Tn ... an )
    {
        // Creates a closure with the critical section parameters
        Critical_Section<Tn ...>* cs = new Critical_Section<Tn ...>(entry, an ...);

        Critical_Section_Base * cur = vouch(cs);
        if (cur != reinterpret_cast<Critical_Section_Base *>(NULL)) do {
            cur->start();
            cur = clear();
        } while (cur != reinterpret_cast<Critical_Section_Base *>(NULL));
    }

    Critical_Section_Base * vouch(Critical_Section_Base * item);
    Critical_Section_Base * clear();

private:
    int _size;

    Critical_Section_Base * _head;
    Critical_Section_Base * _tail;
};

__END_UTIL

#endif
