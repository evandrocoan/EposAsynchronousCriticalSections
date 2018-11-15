// EPOS OStream Implementation

#include <utility/ostream.h>
#include <utility/stringstream.h>

__BEGIN_UTIL

OStream & OStream::operator<<(const StringStream * stream) {
    return operator<<(stream->buffer());
}

__END_UTIL
