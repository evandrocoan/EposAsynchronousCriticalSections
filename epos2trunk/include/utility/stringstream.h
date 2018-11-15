// EPOS StringStream Interface

#include <utility/ostream.h>
#include <utility/string.h>
#include <utility/debug_sync.h>

#ifndef __stringstream_h
#define __stringstream_h

__BEGIN_UTIL


class StringStream: public OStream
{
private:
    char* _buffer;
    unsigned int _last_position;
    const unsigned int _buffer_size;

public:
    StringStream(const unsigned int _buffer_size) :
            OStream::OStream(), _last_position(0), _buffer_size(_buffer_size)
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
        print(stream.buffer());
        return *this;
    }
    StringStream & operator<<(const StringStream * stream) {
        print(stream->buffer());
        return *this;
    }

    void print(const char* string) {
        DB( Synchronizer, TRC, "StringStream::print(this="
                << reinterpret_cast<int *>(this)
                << "), string=" << string << ", " )

        unsigned int string_size = strlen(string);
        unsigned int total_size = string_size + _last_position;

        DB( Synchronizer, TRC, "string_size=" << string_size << ", "
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
        DB( Synchronizer, TRC, ", _last_position=" << _last_position
                << ", _buffer=" << _buffer << endl )
    }

public:
    StringStream & operator<<(const Begl & begl) {
        return *this;
    }

    StringStream & operator<<(const Endl & endl) {
        print("\n");
        _base = 10;
        return *this;
    }

    StringStream & operator<<(const Hex & hex) {
        _base = 16;
        return *this;
    }
    StringStream & operator<<(const Dec & dec) {
        _base = 10;
        return *this;
    }
    StringStream & operator<<(const Oct & oct) {
        _base = 8;
        return *this;
    }
    StringStream & operator<<(const Bin & bin) {
        _base = 2;
        return *this;
    }

    StringStream & operator<<(char c) {
        char buf[2];
        buf[0] = c;
        buf[1] = '\0';
        print(buf);
        return *this;
    }
    StringStream & operator<<(unsigned char c) {
        return operator<<(static_cast<unsigned int>(c));
    }

    StringStream & operator<<(int i) {
        char buf[64];
        buf[itoa(i, buf)] = '\0';
        print(buf);
        return *this;
    }
    StringStream & operator<<(short s) {
        return operator<<(static_cast<int>(s));
    }
    StringStream & operator<<(long l) {
        return operator<<(static_cast<int>(l));
    }

    StringStream & operator<<(unsigned int u) {
        char buf[64];
        buf[utoa(u, buf)] = '\0';
        print(buf);
        return *this;
    }
    StringStream & operator<<(unsigned short s) {
        return operator<<(static_cast<unsigned int>(s));
    }
    StringStream & operator<<(unsigned long l) {
        return operator<<(static_cast<unsigned int>(l));
    }

    StringStream & operator<<(long long int u) {
        char buf[64];
        buf[llitoa(u, buf)] = '\0';
        print(buf);
        return *this;
    }

    StringStream & operator<<(unsigned long long int u) {
        char buf[64];
        buf[llutoa(u, buf)] = '\0';
        print(buf);
        return *this;
    }

    StringStream & operator<<(const void * p) {
        char buf[64];
        buf[ptoa(p, buf)] = '\0';
        print(buf);
        return *this;
    }

    StringStream & operator<<(const char * s) {
        print(s);
        return *this;
    }

    StringStream & operator<<(float f) {
        if(f < 0.0001f && f > -0.0001f)
            (*this) << "0.0000";

        int b = 0;
        int m = 0;

        float x = f;
        if(x >= 0.0001f) {
            while(x >= 1.0000f) {
                x -= 1.0f;
                b++;
            }
            (*this) << b << ".";
            for(int i = 0; i < 3; i++) {
                m = 0;
                x *= 10.0f;
                while(x >= 1.000f) {
                    x -= 1.0f;
                    m++;
                }
                (*this) << m;
            }
        } else {
            while(x <= -1.000f) {
                x += 1.0f;
                b++;
            }
            (*this) << "-" << b << ".";
            for(int i = 0; i < 3; i++) {
                m = 0;
                x *= 10.0f;
                while(x <= -1.000f) {
                    x += 1.0f;
                    m++;
                }
                (*this) << m;
            }
        }
        return *this;
    }
};


__END_UTIL

#endif
