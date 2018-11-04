// EPOS Guard Component Declarations

#ifndef __guard_h
#define __guard_h

#include <architecture/ia32/cpu.h>
#include <utility/closure.h>

__BEGIN_UTIL

class Guard
{
public:
    typedef Closure_Base::Element Element;

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
        Closure<void(Tn ...)>* cs = new (SYSTEM) Closure<void(Tn ...)>(*entry, an ...);

        Element * cur = vouch(&(cs->_link));
        if (cur != reinterpret_cast<Element *>(NULL)) do {
            cur->object()->run();
            cur = clear();
        } while (cur != reinterpret_cast<Element *>(NULL));
    }

    Element * vouch(Element * item);
    Element * clear();

private:
    Element * _head;
    Element * _tail;
};

__END_UTIL

#endif
