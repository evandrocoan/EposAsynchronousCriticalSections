// EPOS Guard Component Declarations

#ifndef __future_h
#define __future_h

#include <condition.h>

__BEGIN_UTIL

template<typename FutureType>
class Future
{
public:
    Future(): _condition(), _is_resolved() {
        db<Synchronizer>(TRC)   << "Future(_is_resolved=" << _is_resolved
                                << ", _condition=" << _condition.size()
                                << ") => " << this << endl;
    }

    ~Future() {
        db<Synchronizer>(TRC) << "~Future(this=" << this << ")" << endl;
    }

    FutureType get_value() {
        db<Synchronizer>(TRC) << "Future::get_value(this=" << this
                              << " _is_resolved=" << _is_resolved
                              << " _condition=" << _condition.size()
                              <<  ")" << endl;
        if(!_is_resolved) _condition.wait();
        return _value;
    }

    void resolve(FutureType value) {
        db<Synchronizer>(TRC) << "Future::resolve(this=" << this
                              << " _is_resolved=" << _is_resolved
                              << " _condition=" << _condition.size()
                              <<  ")" << endl;
        assert(!_is_resolved);
        // If `resolve()` was called and the instruction pointer got until here,
        // and the thread is unscheduled, and another thread call `resolve()`,
        // then, the `assert` above will not work.
        _value = value;
        _is_resolved = true;
        _condition.broadcast();
    }

private:
    bool _is_resolved;
    FutureType _value;
    Condition _condition;
};

__END_UTIL

#endif
