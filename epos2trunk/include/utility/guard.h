// EPOS Guard Component Declarations

#ifndef __guard_h
#define __guard_h

#include <architecture/ia32/cpu.h>
#include <utility/critical_section.h>
#include <utility/debug_sync.h>

__BEGIN_UTIL

class Guard
{

public:
    typedef void (Function)();

public:   
    int _size;
    Thread* _sequencer;

private:
    Critical_Section_Base * _head;
    Critical_Section_Base * _tail;

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
            _sequencer = Thread::self();
            cur->start();
            cur = clear();
        } while (cur != reinterpret_cast<Critical_Section_Base *>(NULL));
    }

    void submit(Function handler)
    {
        Void_Critical_Section * cs = new Void_Critical_Section(handler);

        Critical_Section_Base * volatile cur = vouch(cs);
        if (cur != reinterpret_cast<Critical_Section_Base *>(NULL)) do {
            _sequencer = Thread::self();
            cur->start();
            cur = clear();
        } while (cur != reinterpret_cast<Critical_Section_Base *>(NULL));
    }


    Critical_Section_Base * vouch(Critical_Section_Base * item);
    Critical_Section_Base * clear();
};

__END_UTIL

#endif
