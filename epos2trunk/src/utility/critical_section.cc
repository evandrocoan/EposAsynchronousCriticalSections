// EPOS Guard Component Implementation

#include <utility/critical_section.h>

__BEGIN_UTIL

Critical_Section_Base::Critical_Section_Base(): _link(this) 
{
    db<Synchronizer>(TRC) << "Critical_Section_Base(_link=" << _link << ") => " << this << endl;
}

Critical_Section_Base::~Critical_Section_Base()
{
    db<Synchronizer>(TRC) << "~Critical_Section_Base(this=" << this << " _link=" << _link << ")" << endl;
}

__END_UTIL
