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
    db<Synchronizer>(TRC)   << "~Guard(this=" << this 
                            << ", size=" << _size
                            << ")" << endl;
}

Guard::Element * Guard::vouch(Element * item)
{
    CPU::finc(_size);
    db<Synchronizer>(TRC)   << "Guard::vouch(this=" << this 
                            << " head= " << _head 
                            << " tail= " << _tail 
                            << " size= " << _size 
                            << " cs= " << item->object() 
                            << ")" << endl;
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
    CPU::fdec(_size);
    db<Synchronizer>(TRC)   << "Guard::clear(this=" << this 
                            << " head= " << _head 
                            << " tail= " << _tail 
                            << " size= " << _size 
                            << ")" << endl;

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

    db<Synchronizer>(TRC)   << "Guard::submt(this=" << this
                            << " head= " << _head
                            << " tail= " << _tail
                            << " size= " << _size
                            << " cs= " << cs
                            << ")" << endl;
}

__END_SYS