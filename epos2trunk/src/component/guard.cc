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
    if (last){ 
        if (CPU::cas(last->_next, _null, item) == 0)
            return 0;
        item->destroy();
    }
    _head = item;        
    return item;
}

Guard::Element * Guard::clear()
{
    db<Synchronizer>(TRC) << "Guard::clear(this=" << this << " head= " << _head << " tail= " << _tail <<  ")" << endl;
    Element * item = _head;
    Element * next = CPU::fas(item->_next, _done);
    bool mine = true;
    if (!next)
        mine = CPU::cas(_tail, item, _null) == item;
    CPU::cas(_head, item, next);
    if (mine)
        item->destroy();
    return next;    
}

// Class Methods
void Guard::submit(Critical_Section * cs)
{
    Element * cur = vouch(&(cs->_link));
    if (cur != 0) do {
        cur->object()->run();
        cur = clear();
    } while (cur != 0);    
}

__END_SYS