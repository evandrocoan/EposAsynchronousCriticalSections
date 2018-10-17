// EPOS Guard Component Implementation

#include <guard.h>

__BEGIN_SYS

// Object Methods
Guard::Guard(): _head(0), _tail(0), _null(0), _done(reinterpret_cast<Element *>(1))
{
    db<Synchronizer>(TRC)   << "Guard(head=" << _head 
                            << ", tail=" << _tail 
                            << ", null=" << _null 
                            << ", done=" << _done  
                            << ") => " << this << endl;
}

Guard::~Guard()
{
    db<Synchronizer>(TRC) << "~Guard(this=" << this << ")" << endl;
}

Guard::Element * Guard::vouch(Element * item)
{
    db<Synchronizer>(TRC) << "Guard::vouch(this=" << this << " head= " << _head << " tail= " << _tail <<  ")" << endl;
    item->next(0);
    Element * last = CPU::fas(_tail, item);
    if (last && (CPU::cas(last->_next, _null, item) == 0))
        return 0;
    _head = item;        
    return item;
}

Guard::Element * Guard::clear()
{
    db<Synchronizer>(TRC) << "Guard::clear(this=" << this << " head= " << _head << " tail= " << _tail <<  ")" << endl;

    Element * item = _head;
    Element * next = CPU::fas(item->_next, _done);
    if (!next)
        CPU::cas(_tail, item, _null);
    CPU::cas(_head, item, next);
    return next;    
}

// Class Methods
void Guard::submit(Critical_Section * cs)
{
    Element * cur = vouch(&(cs->_link));
    if (0 != cur) do {
        cur->object->run();
    } while (0 != (cur = clear()));    
}

__END_SYS