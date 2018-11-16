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
    Future(): _condition(), _is_resolved() {
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

        if(!_is_resolved) {
            _condition.wait();
        }
        return _value;
    }

    void resolve(FutureType value) {
        DB( Synchronizer, TRC, "Future::resolve(this=" << this
                              << " _is_resolved=" << _is_resolved
                              << " _condition=" << _condition.size()
                              <<  ")" << endl )
        assert(!_is_resolved);
        // If `resolve()` was called and the instruction pointer got until here,
        // and the thread is unscheduled, and another thread call `resolve()`,
        // then, the `assert` will not work, if the resolve() call is not atomic.
        _value = value;
        _is_resolved = true;
        _condition.broadcast();
    }

private:
    bool _is_resolved;

    Condition _condition;
    FutureType _value;
};

__END_UTIL

#endif
