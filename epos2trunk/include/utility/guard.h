// EPOS Guard Component Declarations

#ifndef __guard_h
#define __guard_h

#include <architecture/ia32/cpu.h>
#include <utility/list.h>
#include <utility/handler.h>
#include <utility/future.h>

__BEGIN_UTIL


template<typename FutureType>
class Critical_Section
{
public:
    friend class Guard;
    // typedef Handler::Function Function;
    typedef void (Function)(Future<FutureType>*);
    typedef List_Elements::Singly_Linked<Critical_Section> Element;

public:
    Critical_Section(Function * h, Future<FutureType>* future): _handler(h), _future(future), _link(this) {}
    ~Critical_Section() {}

    void operator()() { run(); } // Alias for ()
    void run()        { _handler(_future); } 

private:
    Function* _handler;
    Future<FutureType>* _future;
    Element _link; // Inspired by the thread code
};

class Guard
{
public:
    typedef Critical_Section<int>::Element Element;

private:
    static const int NULL = 0;
    static const int DONE = 1;

public:
    Guard();
    ~Guard();

    void submit(Critical_Section<int> * cs);
    Element * vouch(Element * item);
    Element * clear();

private:
    Element * _head;
    Element * _tail;
};

__END_UTIL

#endif
