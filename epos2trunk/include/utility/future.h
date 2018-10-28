// EPOS Guard Component Declarations

#ifndef __future_h
#define __future_h

#include <semaphore.h>

__BEGIN_UTIL

template<typename FutureType>
class Future 
{
public:
    Future(): _semaphore(0), _is_resolved() {
        db<Synchronizer>(WRN)   << "Future(is_resolved=" << _is_resolved 
                                << ", _semaphore=" << _semaphore.size()
                                << ") => " << this << endl;
    }

    ~Future() {
        db<Synchronizer>(WRN) << "~Future(this=" << this << ")" << endl;
    }

    FutureType get_value() {
        db<Synchronizer>(WRN) << "Future::get_value(this=" << this 
                              << " is_resolved=" << _is_resolved 
                              << " semaphore=" << _semaphore.size()
                              <<  ")" << endl;
        if(!_is_resolved) _semaphore.p();
        return _value;
    }

    void resolve(FutureType value) { 
        db<Synchronizer>(WRN) << "Future::resolve(this=" << this 
                              << " is_resolved=" << _is_resolved 
                              << " semaphore=" << _semaphore.size()
                              <<  ")" << endl;
        assert(!_is_resolved);
        _is_resolved = true;
        _value = value;
        _semaphore.v();
    }

private:
    bool _is_resolved;
    FutureType _value;
    Semaphore _semaphore;
};

__END_UTIL

#endif
