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
        db<Synchronizer>(WRN)   << "Future(_is_resolved=" << _is_resolved 
                                << ", _condition=" << _condition.size()
                                << ") => " << this << endl;
    }

    ~Future() {
        db<Synchronizer>(WRN) << "~Future(this=" << this << ")" << endl;
    }

    FutureType get_value() {
        db<Synchronizer>(WRN) << "Future::get_value(this=" << this 
                              << " _is_resolved=" << _is_resolved 
                              << " _condition=" << _condition.size()
                              <<  ")" << endl;
        if(!_is_resolved) _condition.wait();
        return _value;
    }

    void resolve(FutureType value) { 
        db<Synchronizer>(WRN) << "Future::resolve(this=" << this 
                              << " _is_resolved=" << _is_resolved 
                              << " _condition=" << _condition.size()
                              <<  ")" << endl;
        assert(!_is_resolved);
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
