// EPOS Guard Component Implementation

#include <utility/guard.h>

__BEGIN_UTIL

Guard::Guard(): _head(0), _tail(0) {
    DB( Synchronizer, TRC, "Guard(head=" << _head << ", tail=" << _tail
            << ") => " << this << endl )
}

Guard::~Guard() {
    DB( Synchronizer, TRC, "~Guard(this=" << this << ")" << endl )
}

Guard::Element * Guard::vouch(Element * item)
{
    DB( Synchronizer, TRC, "Guard::vouch(this=" << this
            << " head=" << _head << " tail=" << _tail
            << " item=" << item << ", size=" << _size + 1 )
    CPU::finc(_size);

    item->next( reinterpret_cast<Element *>(NULL) );
    Element * last = CPU::fas(_tail, item);
    DB( Synchronizer, TRC, ", last=" << last << ")" << endl )

    if( last ) {
        if( CPU::cas( last->_next, reinterpret_cast<Element *>(NULL), item )
                    == reinterpret_cast<Element *>(NULL) )

            return reinterpret_cast<Element *>(NULL);
        delete item->object();
    }
    _head = item;
    return item;
}

Guard::Element * Guard::clear()
{
    DB( Synchronizer, TRC, "Guard::clear(this=" << this
            << " head=" << _head << " tail=" << _tail
            << ", size=" << _size - 1 )
    CPU::fdec(_size);

    Element * item = _head;
    Element * next = CPU::fas( item->_next, reinterpret_cast<Element *>(DONE) );
    DB( Synchronizer, TRC, ", next=" << next << ")" << endl )

    bool mine = true;
    if( !next ) {
        assert(!_size);
        mine = CPU::cas( _tail, item, reinterpret_cast<Element *> (NULL) ) == item;
    }

    CPU::cas(_head, item, next);
    if( mine ) {
        delete item->object();
    }
    return next;
}

__END_UTIL
