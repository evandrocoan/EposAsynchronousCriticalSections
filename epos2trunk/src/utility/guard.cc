// EPOS Guard Component Implementation

#include <utility/guard.h>

__BEGIN_UTIL

Guard::Guard(): _head(0), _tail(0) {
    LOG( Guard, TRC, "Guard(head=" << _head << ", tail=" << _tail
            << ") => " << this << endl )
}

Guard::~Guard() {
    LOG( Guard, TRC, "~Guard(this=" << this << ")" << endl )
}

Guard::Element * Guard::vouch(Element * item)
{
    LOG( Guard, TRC, "Guard::vouch(this=" << this
            << " head=" << _head << " tail=" << _tail <<  ")" << endl )

    item->next(reinterpret_cast<Element *>(NULL));
    Element * last = CPU::fas(_tail, item);
    if (last) {
        if (CPU::cas(last->_next, reinterpret_cast<Element *>(NULL), item)
                    == reinterpret_cast<Element *>(NULL))
            return reinterpret_cast<Element *>(NULL);
        delete item->object();
    }
    _head = item;
    return item;
}

Guard::Element * Guard::clear()
{
    LOG( Guard, TRC, "Guard::clear(this=" << this
            << " head=" << _head << " tail=" << _tail <<  ")" << endl )

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

__END_UTIL
