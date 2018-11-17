// EPOS Spin Lock Utility Declarations

#ifndef __spin_h
#define __spin_h

#include <cpu.h>

__BEGIN_UTIL

// Forwarder to the running thread id
class This_Thread
{
public:
    static unsigned int id();
    static void not_booting() { _not_booting = true; }

private:
    static bool _not_booting;
};

// Recursive Spin Lock
class Spin
{
public:
    Spin(): _level(0), _owner(0) {}

    void acquire() {
        int me = This_Thread::id();

        while(CPU::cas(_owner, 0, me) != me);
        _level++;

        db<Spin>(TRC) << "Spin::acquire[SPIN=" << this << ",ID=" << reinterpret_cast<void *>(me)
                << "]() => {owner=" << owner() << ",level=" << level() << "}" << endl;
    }

    void release() {
        auto me = owner();
    	if(--_level <= 0)
            _owner = 0;

        db<Spin>(TRC) << "Spin::release[SPIN=" << this << ",ID=" << me
                << "]() => {owner=" << owner() << ",level=" << level() << "}" << endl;
    }

    inline const void * owner() const { return reinterpret_cast<void *>(_owner); }
    inline const int level() const { return _level; }

private:
    volatile int _level;
    volatile int _owner;
};

// Flat Spin Lock
class Simple_Spin
{
public:
    Simple_Spin(): _locked(false) {}

    void acquire() {
        while(CPU::tsl(_locked));

        db<Spin>(TRC) << "Spin::acquire[SPIN=" << this << "]()" << endl;
    }

    void release() {
//        if(_locked)
            _locked = 0;

        db<Spin>(TRC) << "Spin::release[SPIN=" << this << "]()}" << endl;
    }

private:
    volatile bool _locked;
};

__END_UTIL

#endif
