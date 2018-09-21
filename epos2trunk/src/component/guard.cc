// EPOS Guard Component Implementation

#include <guard.h>

__BEGIN_SYS

Guard::Guard(): _head(0), _tail(0), _null(0), _done()
{
    db<Synchronizer>(TRC) << "Guard(head=" << _head << ", tail=" << _tail << ") => " << this << endl;
}

Guard::~Guard()
{
    db<Synchronizer>(TRC) << "~Guard(this=" << this << ")" << endl;
}

Guard::Element * Guard::vouch(Element * item)
{
    db<Synchronizer>(TRC) << "Guard::vouch(this=" << this << ", item=" << item <<", tail=" << _tail << ")" << endl;
    item->next(_null);
    Element * last = CPU::cas(_tail, _tail, item);
    if (last){
        if (CPU::cas(last->_next, _null, item))
            return _null;
    }
    _head = item;        
    return item;
}

Guard::Element * Guard::clear()
{
    db<Synchronizer>(TRC) << "Guard::clear(this=" << this << ",head=" << _head << ")" << endl;
    Element * item = _head;
    Element * next = CPU::cas(item->_next, item->_next, _done);
    if (!next)
        CPU::cas(_tail, item, _null);
    CPU::cas(_head, item, next);
    return next;    
}

__END_SYS