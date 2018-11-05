// EPOS Guard Component Declarations

#ifndef __critical_section_h
#define __critical_section_h

#include <utility/list.h>
#include <utility/closure.h>

__BEGIN_UTIL

class Critical_Section_Base
{
    /// The Guard class requires access to our the _link private attribute
    friend class Guard;
    typedef List_Elements::Singly_Linked<Critical_Section_Base> Element;

public:
    Critical_Section_Base(): _link(this) {
        db<Synchronizer>(TRC) << "Critical_Section_Base(_link=" << &_link << ") => " << this << endl;
    }

    /// This must to be virtual otherwise the derived classes objects destructor method would not be
    /// called when accessed by a base class pointer.
    virtual ~Critical_Section_Base() {
        db<Synchronizer>(TRC) << "~Critical_Section_Base(this=" << this << " _link=" << &_link << ")" << endl;
    }

    /// Returns void because the base class Critical_Section_Base() cannot be a template class, as it is
    /// general interface for all Closures used on the Guard algorithm.
    /// Also, it does not make sense to return something from the closure when in
    ///
    /// This must to be virtual otherwise the derived classes objects run() method would not be
    /// called when accessed by a base class pointer.
    virtual void start() = 0;

private:
    // Inspired by the thread code
    Element _link;
};

template<typename... Tn>
class Critical_Section: public Critical_Section_Base, private Closure<void( Tn... )>
{
    using Closure<void( Tn... )>::run;

public:
    Critical_Section(void(*_entry)(Tn ...), Tn ... an) : Closure<void( Tn... )>::Closure( _entry, an ... )
    {
    }

    inline void start() {
        run();
    }
};

__END_UTIL

#endif
