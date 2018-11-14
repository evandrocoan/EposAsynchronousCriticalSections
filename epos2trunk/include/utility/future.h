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
        LOG( Synchronizer, TRC, "Future(_is_resolved=" << _is_resolved
                  << ", _condition=" << _condition.size()
                  << ") => " << this << endl )
    }

    ~Future() {
        LOG( Synchronizer, TRC, "~Future(this=" << this << ")" << endl );
    }

    FutureType get_value() {
        LOG( Synchronizer, TRC, "Future::get_value(this=" << this
                              << " _is_resolved=" << _is_resolved
                              << " _condition=" << _condition.size()
                              <<  ")" << endl )
        _lock.lock();

        if(!_is_resolved) {
            _lock.unlock();
            _condition.wait();
        }
        else {
            _lock.unlock();
        }
        return _value;
    }

    void resolve(FutureType value) {
        LOG( Synchronizer, TRC, "Future::resolve(this=" << this
                              << " _is_resolved=" << _is_resolved
                              << " _condition=" << _condition.size()
                              <<  ")" << endl )
        _lock.lock();
        assert(!_is_resolved);
        // If `resolve()` was called and the instruction pointer got until here,
        // and the thread is unscheduled, and another thread call `resolve()`,
        // then, the `assert` will not work, if the resolve() call is not atomic.
        _value = value;
        _is_resolved = true;
        _condition.broadcast();
        _lock.unlock();
    }

private:
    bool _is_resolved;
    Semaphore _lock;

    Condition _condition;
    FutureType _value;
};

__END_UTIL

#endif
