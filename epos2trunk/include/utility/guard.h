// EPOS Guard Component Declarations

#ifndef __guard_h
#define __guard_h

#include <architecture/ia32/cpu.h>
#include <utility/critical_section.h>
#include <utility/debug_sync.h>

__BEGIN_UTIL

class Guard
{
public:
    typedef void (Function)();

private:
    int volatile _size;
    Thread* _sequencer;

    Critical_Section_Base * volatile _head;
    Critical_Section_Base * volatile _tail;

    static const int NULL = 0;
    static const int DONE = 1;

public:
    static Guard * current_guard;                                                                     
    static int volatile size; // 16725490 => single thread 
    static int v;
    static void * cur_item;

    static void pf_cs();
    static void show_size();

public:
    Guard();
    ~Guard();

    template<typename ... Tn>
    void submit( void(*entry)( Tn ... ), Tn ... an )
    {
        // Creates a closure with the critical section parameters
        Critical_Section<Tn ...>* cs = new Critical_Section<Tn ...>(entry, an ...);

        Critical_Section_Base * volatile cur = vouch(cs, 322, false);
        if (cur != reinterpret_cast<Critical_Section_Base *>(NULL)) do {
            _sequencer = Thread::self();
            cur->start();
            cur = clear();
        } while (cur != reinterpret_cast<Critical_Section_Base *>(NULL));
    }

    void submit(Function handler, int arg)
    {
        Void_Critical_Section * cs = new Void_Critical_Section(handler);

        Critical_Section_Base * volatile cur = vouch(cs, arg, false);
        if (cur != reinterpret_cast<Critical_Section_Base *>(NULL)) do {
            _sequencer = Thread::self();
            cur->start();
            cur = clear();
        } while (cur != reinterpret_cast<Critical_Section_Base *>(NULL));
    }

    void submit_no_sequencer(Function handler, int arg)
    {
        vouch(new Void_Critical_Section(handler), arg, true);
    }

    void show(){
        int count = 0;
        Critical_Section_Base * cur = _head;

        LOG("[SHOW BEGIN]");
        if(cur != NULL){
            do {
                LOG("\n[" << count << "/" << _size << "][" << cur << "]");
                count++;
                cur = cur->_next;
            } while (cur->_next != 0);
            LOG(" -> [" << cur->_next << "]" << endl);
            LOG("[SHOW END]")
        } else {
            LOG("\n[HEAD][" << _head << "]  [TAIL][" << _tail << "]" << endl);
            LOG("[SHOW END]")
        }

    }

    Critical_Section_Base * vouch(Critical_Section_Base * volatile item, int arg, bool nosq);
    Critical_Section_Base * clear();
};

__END_UTIL

#endif
