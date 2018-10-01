// EPOS Guard Component Declarations

#ifndef __guard_h
#define __guard_h

#include <architecture/ia32/cpu.h>
#include <utility/list.h>
#include <utility/handler.h>

__BEGIN_SYS

class Guard
{
public:
    // Article's chain_t
    typedef List_Elements::Singly_Linked<Function_Handler> Element;

public:
    Guard();
    ~Guard();

    Element * vouch(Element * item);
    Element * clear();

private:
    Element * _head;
    Element * _tail;
    Element * _null;
    Element * _done;
};

__END_SYS

#endif

