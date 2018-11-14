// EPOS StringStream Interface

#include <utility/ostream.h>
#include <utility/string.h>
#include <utility/debug_sync.h>

#ifndef __stringstream_h
#define __stringstream_h

__BEGIN_UTIL


class StringStream: public OStream
{
    char* _buffer;
    unsigned int _last_position;
    const unsigned int _buffer_size;

public:
    using OStream::operator<<;

    StringStream(const unsigned int _buffer_size) :
            OStream::OStream(), _last_position(0), _buffer_size(_buffer_size)
    {
        assert(_buffer_size > 0);
        LOG( Synchronizer, TRC, "StringStream::StringStream(_buffer_size="
                << _buffer_size << ") => " << reinterpret_cast<int *>(this) << endl )

        _buffer = new char[_buffer_size];
    }

    ~StringStream() {
        LOG( Synchronizer, TRC, "StringStream::~StringStream(this="
                << reinterpret_cast<int *>(this) << ", _buffer="
                << reinterpret_cast<int *>(_buffer) << ")" << endl )

        delete _buffer;
    }

    const char * const buffer() const {
        return _buffer;
    }

    StringStream & operator<<(const StringStream & stream) {
        print(stream.buffer());
        return *this;
    }

    void print(const char* string) {
        LOG( Synchronizer, TRC, "StringStream::print(this="
                << reinterpret_cast<int *>(this)
                << "), string=" << string << ", " )

        unsigned int string_size = strlen(string);
        unsigned int total_size = string_size + _last_position;

        LOG( Synchronizer, TRC, "string_size=" << string_size << ", "
                << "total_size=" << total_size )

        // https://linux.die.net/man/3/strncpy
        if( total_size >= _buffer_size ) {
            total_size = _buffer_size - 1;

            strncpy(&_buffer[_last_position], string, total_size - _last_position);
            _buffer[total_size] = '\0';
        }
        else {
            strcpy(&_buffer[_last_position], string);
        }

        _last_position = total_size;
        LOG( Synchronizer, TRC, ", _last_position=" << _last_position
                << ", _buffer=" << _buffer << endl )
    }
};


__END_UTIL

#endif
