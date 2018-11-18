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

Critical_Section_Base* Guard::vouch(Critical_Section_Base * item)
{
    DB( Synchronizer, TRC, "Guard::vouch(this=" << this
            << " head=" << _head << " tail=" << _tail
            << " item=" << item << ", size=" << _size + 1 )
    CPU::finc(_size);

    item->_next = reinterpret_cast<Critical_Section_Base *>(NULL);
    Critical_Section_Base * last = CPU::fas(_tail, item);
    DB( Synchronizer, TRC, ", last=" << last << ")" << endl )

    if( last ) {
        if( CPU::cas( last->_next, reinterpret_cast<Critical_Section_Base *>(NULL), item )
                    == reinterpret_cast<Critical_Section_Base *>(NULL) )

            return reinterpret_cast<Critical_Section_Base *>(NULL);
        delete last;
    }
    _head = item;
    return item;
}

Critical_Section_Base* Guard::clear()
{
    DB( Synchronizer, TRC, "Guard::clear(this=" << this
            << " head=" << _head << " tail=" << _tail
            << ", size=" << _size - 1 )
    CPU::fdec(_size);

    Critical_Section_Base * item = _head;
    Critical_Section_Base * next = CPU::fas( item->_next, reinterpret_cast<Critical_Section_Base *>(DONE) );
    DB( Synchronizer, TRC, ", next=" << next << ")" << endl )

    bool mine = true;
    if( !next ) {
        assert(!_size);
        mine = CPU::cas( _tail, item, reinterpret_cast<Critical_Section_Base *> (NULL) ) == item;
    }

    CPU::cas(_head, item, next);
    if( mine ) {
        delete item;
    }
    return next;
}

__END_UTIL
