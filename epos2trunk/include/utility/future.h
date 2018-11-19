// EPOS Guard Component Declarations

#ifndef __future_h
#define __future_h

#include <semaphore.h>
#include <condition.h>
#include <utility/debug_sync.h>

__BEGIN_UTIL

template<typename FutureType>
class Future
{
public:
    Future(): _lock(1), _condition(), _is_resolved(false) {
        DB( Synchronizer, TRC, "Future(_is_resolved=" << _is_resolved
                << ", _condition=" << _condition.size()
                << ") => " << this << endl )
    }

    ~Future() {
        DB( Synchronizer, TRC, "~Future(this=" << this << ")" << endl );
    }

    FutureType get_value() {
        DB( Synchronizer, TRC, "Future::get_value(this=" << this
                << " _is_resolved=" << _is_resolved
                << " _condition=" << _condition.size()
                <<  ")" << endl )
        // It can only enter on the if block, if the _is_resolved value is false!
        // if _is_resolved is true, then CPU::cas() will keep it true and return true
        // if _is_resolved is false, then CPU::cas() will keep it false and return false
        if( !CPU::cas( _is_resolved, false, _is_resolved ) ) {
            _condition.wait();
        }
        return _value;
    }

    void resolve(FutureType value) {
        DB( Synchronizer, TRC, "Future::resolve(this=" << this
                << " _is_resolved=" << _is_resolved
                << " _condition=" << _condition.size()
                <<  ")" << endl )
        _lock.lock();
        assert( !_is_resolved );
        // If the instruction pointer got until here, and the thread is unscheduled,
        // and another thread call `resolve()`, then, the `assert` will not work,
        // if the whole resolve() call is not atomic.
        _value = value;

        // atomically resolve the future
        CPU::fas( _is_resolved, true );

        _lock.unlock();
        _condition.broadcast();
    }

private:
    Semaphore _lock;
    volatile bool _is_resolved;

    Condition _condition;
    FutureType _value;
};

__END_UTIL

#endif
