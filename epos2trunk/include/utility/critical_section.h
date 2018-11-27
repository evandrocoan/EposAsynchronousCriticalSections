// EPOS Guard Component Declarations

#ifndef __critical_section_h
#define __critical_section_h

#include <utility/list.h>
#include <utility/closure.h>

__BEGIN_UTIL

class Critical_Section_Base
{
    /// The Guard class requires access to our the _next private attribute
    friend class Guard;

public:
    Critical_Section_Base(): _next(this) {
        DB( Synchronizer, TRC, "Critical_Section_Base(_next=" << &_next
                << ") => " << this << endl )
    }

    /// This must to be virtual otherwise the derived classes objects destructor
    /// method would not be called when accessed by a base class pointer.
    virtual ~Critical_Section_Base() {
        DB( Synchronizer, TRC, "~Critical_Section_Base(this=" << this
                << " _next=" << &_next << ")" << endl )
    }

    /// Returns void because the base class Critical_Section_Base() cannot be a
    /// template class, as it is general interface for all Closures used on the
    /// Guard algorithm. Also, it does not make sense to return something from
    /// the closure when in its code is ran by the sequencer on another thread,
    /// detached from the original code.
    ///
    /// This must to be virtual otherwise the derived classes objects run()
    /// method would not be called when accessed by a base class pointer.
    virtual void start() = 0;

private:
    // Inspired by the thread code
    Critical_Section_Base* _next;
};

template<typename... Tn>
class Critical_Section: public Critical_Section_Base, private Closure<void( Tn... )>
{
    using Closure<void( Tn... )>::run;

public:
    Critical_Section(void(*_entry)(Tn ...), Tn ... an) :
            Closure<void( Tn... )>::Closure( _entry, an ... )
    {
        DB( Synchronizer, TRC, "Critical_Section(_entry="
                << reinterpret_cast<void *>(_entry) << ") => " << this << endl )
    }

    inline void start() {
        run();
    }
};

class Void_Critical_Section: public Critical_Section_Base
{

public:
    typedef void (Function)();

public:
    Void_Critical_Section(Function * h): _handler(h) {}
    
    inline void start() {
        _handler();
    }

private:
    Function * _handler;
};

__END_UTIL

#endif
