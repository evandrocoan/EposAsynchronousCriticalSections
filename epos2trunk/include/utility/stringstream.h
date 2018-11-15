// EPOS StringStream Interface

#include <utility/ostream.h>
#include <utility/string.h>
#include <utility/debug_sync.h>

#ifndef __stringstream_h
#define __stringstream_h

__BEGIN_UTIL

class StringStream : public OStream_Base<StringStream>
{
private:
    char* _buffer;
    unsigned int _last_position;
    const unsigned int _buffer_size;

public:
    StringStream(const unsigned int _buffer_size) :
            _last_position(0), _buffer_size(_buffer_size)
    {
        assert(_buffer_size > 0);
        DB( Synchronizer, TRC, "StringStream::StringStream(_buffer_size="
                << _buffer_size << ") => " << reinterpret_cast<int *>(this) << endl )

        _buffer = new char[_buffer_size];
    }

    ~StringStream() {
        DB( Synchronizer, TRC, "StringStream::~StringStream(this="
                << reinterpret_cast<int *>(this) << ", _buffer="
                << reinterpret_cast<int *>(_buffer) << ")" << endl )

        delete _buffer;
    }

    const char * const buffer() const {
        return _buffer;
    }

    StringStream & operator<<(const StringStream & stream) {
        OStream_Base<StringStream>::print(stream.buffer());
        return *this;
    }
    StringStream & operator<<(const StringStream * stream) {
        OStream_Base<StringStream>::print(stream->buffer());
        return *this;
    }

    static void print(StringStream* that, const char* string) {
        DB( Synchronizer, TRC, "StringStream::print(this="
                << reinterpret_cast<int *>(that)
                << "), string=" << string << ", " )

        unsigned int string_size = strlen(string);
        unsigned int total_size = string_size + that->_last_position;

        DB( Synchronizer, TRC, "string_size=" << string_size << ", "
                << "total_size=" << total_size )

        // https://linux.die.net/man/3/strncpy
        if( total_size >= that->_buffer_size ) {
            total_size = that->_buffer_size - 1;

            strncpy(&that->_buffer[that->_last_position], string, total_size - that->_last_position);
            that->_buffer[total_size] = '\0';
        }
        else {
            strcpy(&that->_buffer[that->_last_position], string);
        }

        that->_last_position = total_size;
        DB( Synchronizer, TRC, ", _last_position=" << that->_last_position
                << ", _buffer=" << that->_buffer << endl )
    }

public:
    StringStream & operator<<(const Begl & begl) {
        return *this;
    }

    StringStream & operator<<(const Endl & endl) {
        OStream_Base<StringStream>::print("\n");
        _set_base(10);
        return *this;
    }

    using OStream_Base<StringStream>::operator<<;
};

__END_UTIL

#endif
