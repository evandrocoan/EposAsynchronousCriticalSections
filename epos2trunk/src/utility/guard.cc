// EPOS Guard Component Implementation

#include <utility/guard.h>

__BEGIN_UTIL

Guard * Guard::current_guard;
int volatile Guard::size;
int Guard::v;
void * Guard::cur_item;

void Guard::pf_cs(){
    LOG("[SOMETHING HAPPENED]" << endl << "[guard= " <<  current_guard << "][size= " << size << "]" << endl);
}

Guard::Guard(): _size(0), _sequencer(0), _head(0), _tail(0) {
    DB( Synchronizer, TRC, "Guard(head=" << _head << ", tail=" << _tail
            << ") => " << this << endl )
    current_guard = this;
    v = 322;
}

void Guard::show_size(){
    db<Scheduler<Synchronizer> >(WRN) << "[v= " << v << "]" << "[item= " << cur_item << "]" << endl;
}

Guard::~Guard() {
    DB( Synchronizer, TRC, "~Guard(this=" << this << ")" << endl )
}

Critical_Section_Base* Guard::vouch(Critical_Section_Base * volatile item, int arg, bool nosq)
{
    DB( Synchronizer, TRC, "Guard::vouch(this=" << this
            << " head=" << _head
            << " tail=" << _tail
            << " thread=" << Thread::self()
            << " sequencer=" << _sequencer
            << " size=" << _size + 1
            << " item=" << item )
            
    CPU::finc(size);
    cur_item = reinterpret_cast<void *>(item);

    v = arg * 1 + 1; // v1

    item->_next = reinterpret_cast<Critical_Section_Base *>(NULL);
    Critical_Section_Base * volatile last = CPU::fas(_tail, item);

    /* Debug variables start */
     Critical_Section_Base * volatile tail_after_v2 = _tail;
    Critical_Section_Base * volatile last_before_v3 = last;
     Critical_Section_Base * volatile last_next_before_v3 = last->_next;
    /* Debug variables end */
    v = arg * 2 + 2; // v2
    if(last) {
        v = arg * 3 + 3; // v3
        /* Debug Variables start */
        Critical_Section_Base * volatile last_after_v3 = last;
        Critical_Section_Base * volatile last_next_after_v3 = last->_next;

        /* Debug Variables end */

        if(CPU::cas(last->_next, reinterpret_cast<Critical_Section_Base *>(NULL), item) == reinterpret_cast<Critical_Section_Base *>(NULL)){
            v = arg * 4 + 4; // v4
            return 0;
        }

        /* Debug Messages start  */
        LOG("[SEQUENCER TRANSITION]"                                                                                    << endl <<
        "[          item= "     << item                     << "] "    << "[         item_n= "   <<  item->_next            << "] "      << endl <<
        "[      old_tail= "     << tail_after_v2            << "] "    << "[           tail= "   <<  _tail                  << "] "      << endl <<
        "[  old_last_bv3= "     << last_before_v3           << "] "    << "[  old_last_afv3= "   << last_after_v3           << "] "       << "[      last= "           <<  last   << "] "      << endl <<
        "[old_last_n_bv3= "     << last_next_before_v3      << "] "    << "[old_last_n_afv3= "   << last_next_after_v3      << "] "       << "[last->next= "     <<  last->_next  << "] "      << endl <<
        "[size= "          << size                   << "]"   << endl);
        /* Debug Messages end */
        delete last;
    }
    _head = item;
    return item;
}

Critical_Section_Base* Guard::clear()
{
    DB( Synchronizer, TRC, "Guard::clear(this=" << this
            << " head=" << _head
            << " tail=" << _tail
            << " thread=" << Thread::self()
            << " sequencer=" << _sequencer
            << " size=" << _size - 1 )
    CPU::fdec(size);

    Critical_Section_Base * volatile item = _head;
    Critical_Section_Base * volatile next = CPU::fas( item->_next, reinterpret_cast<Critical_Section_Base *>(DONE) );

    DB( Synchronizer, TRC, " next=" << next << ")" << endl )
    bool mine = true;

    if( !next ) {
        mine = CPU::cas( _tail, item, reinterpret_cast<Critical_Section_Base *> (NULL) ) == item;
    }

    CPU::cas( _head, item, next );
    if( mine ) {
        delete item;
    }
    return next;
}

__END_UTIL
