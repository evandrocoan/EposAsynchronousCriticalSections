// EPOS Guard Component Declarations

#ifndef __guard_h
#define __guard_h

#include <architecture/ia32/cpu.h>
#include <utility/list.h>
#include <utility/handler.h>

__BEGIN_SYS

// Replicated code from list.h, moved here because CAS and FAS need direct accesss to the next atribute
// Simple List Element
template<typename T>
class Singly_Linked
{
public:
    typedef T Object_Type;
    typedef Singly_Linked Element;

public:
    Singly_Linked(const T * o): _object(o), _next(0) {}

    T * object() const { return const_cast<T *>(_object); }

    Element * next() const { return _next; }
    void next(Element * e) { _next = e; }

public:
    const T * _object;
    Element * _next;
};

class Critical_Section
{
public:
    typedef Handler::Function Function;
    typedef Singly_Linked<Critical_Section> Element;

public:
    Critical_Section(Function * h): _handler(h), _link(this) {}
    ~Critical_Section() {}

    void operator()() { _handler(); }
    void run()        { _handler(); } 

public:
    Function * _handler;
    Element _link;
};

class Guard
{
public:
    typedef Critical_Section::Element Element;

private:
   // static const unsigned int NULL = 0;
   // static const unsigned int DONE = 1;

public:
    Guard();
    ~Guard();

    Element * vouch(Element * item);
    Element * clear();
    void submit(Critical_Section * cs);

private:
    Element * _head;
    Element * _tail;
    Element * _null;
    Element * _done;
};


__END_SYS

#endif

