// EPOS Guard Component Implementation

#include <utility/guard.h>

__BEGIN_UTIL

// Object Methods
Guard::Guard(): _head(0), _tail(0)
{
    db<Synchronizer>(TRC)   << "Guard(head=" << _head 
                            << ", tail=" << _tail 
                            << ") => " << this << endl;
}

Guard::~Guard()
{
    db<Synchronizer>(TRC) << "~Guard(this=" << this << ")" << endl;
}

Guard::Element * Guard::vouch(Element * item)
{
    db<Synchronizer>(TRC) << "Guard::vouch(this=" << this << " head=" << _head << " tail=" << _tail <<  ")" << endl;
    item->next(NULL);
    Element * last = CPU::fas(_tail, item);
    if (last) { 
        if (CPU::cas(last->_next, reinterpret_cast<Element *>(NULL), item) == NULL)
            return NULL;
        delete item->object();
    }
    _head = item;
    return item;
}

Guard::Element * Guard::clear()
{
    db<Synchronizer>(TRC) << "Guard::clear(this=" << this << " head=" << _head << " tail=" << _tail <<  ")" << endl;
    Element * item = _head;
    Element * next = CPU::fas(item->_next, reinterpret_cast<Element *>(DONE));
    bool mine = true;
    if (!next)
        mine = CPU::cas(_tail, item, reinterpret_cast<Element *> (NULL)) == item;
    CPU::cas(_head, item, next);
    if (mine)
        delete item->object();
    return next;    
}

// Class Methods
void Guard::submit(Critical_Section * cs)
{
    Element * cur = vouch(&(cs->_link));
    if (cur != NULL) do {
        cur->object()->run();
        cur = clear();
    } while (cur != NULL);
}

__END_UTIL
