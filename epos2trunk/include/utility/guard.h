// EPOS Guard Component Declarations

#ifndef __guard_h
#define __guard_h

#include <architecture/ia32/cpu.h>
#include <utility/list.h>
#include <utility/handler.h>

__BEGIN_UTIL

class Critical_Section
{
public:
    friend class Guard;
    typedef Handler::Function Function;
    typedef List_Elements::Singly_Linked<Critical_Section> Element;

public:
    Critical_Section(Function * h): _handler(h), _link(this) {}
    ~Critical_Section() {}

    void operator()() { _handler(); }
    void run()        { _handler(); } // Alias for ()

private:
    Function * _handler;
    Element _link; // Inspired by the thread code
};

class Guard
{
public:
    typedef Critical_Section::Element Element;

private:
    static const int NULL = 0;
    static const int DONE = 1;

public:
    Guard();
    ~Guard();

    void submit(Critical_Section * cs);
    Element * vouch(Element * item);
    Element * clear();

private:
    Element * _head;
    Element * _tail;
};

__END_UTIL

#endif
