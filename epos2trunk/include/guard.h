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

    void destroy(){ delete _object; }

    Element * next() const { return _next; }
    void next(Element * e) { _next = e; }

private:
    const T * _object;

public:
    // Next had to be made public in order to be used in FAS and CAS
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
    Element _link; // Inspired by the thread code
};

class Guard
{
public:
    typedef Critical_Section::Element Element;

public:
    Guard();
    ~Guard();

    Element * vouch(Element * item);
    Element * clear();
    void submit(Critical_Section * cs);

private:
    int _size;
    Element * _head;
    Element * _tail;
    // _null and _done look very weird here, however, it was
    // the only way to represent a null and a done pointer to a guard element
    // that we managed to get to work with CAS and FAS
    Element * _null;
    Element * _done;
};


__END_SYS

#endif

