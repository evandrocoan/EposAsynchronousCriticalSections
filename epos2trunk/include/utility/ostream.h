// EPOS OStream Interface

#include <system/config.h>

#ifndef __ostream_h
#define __ostream_h

extern "C" {
    void _print_preamble();
    void _print(const char * s);
    void _print_trailler(bool error);
}

__BEGIN_UTIL

template<class StreamType>
class OStream_Base
{
public:
    struct Begl {};
    struct Endl {};

    struct Hex {};
    struct Dec {};
    struct Oct {};
    struct Bin {};

public:
    OStream_Base(): _base(10) {}

    StreamType& operator<<(const Hex & hex) {
        _set_base(16);
        return *static_cast<StreamType*>(this);
    }
    StreamType& operator<<(const Dec & dec) {
        _set_base(10);
        return *static_cast<StreamType*>(this);
    }
    StreamType& operator<<(const Oct & oct) {
        _set_base(8);
        return *static_cast<StreamType*>(this);
    }
    StreamType& operator<<(const Bin & bin) {
        _set_base(2);
        return *static_cast<StreamType*>(this);
    }

    StreamType& operator<<(char c) {
        char buf[2];
        buf[0] = c;
        buf[1] = '\0';
        print(buf);
        return *static_cast<StreamType*>(this);
    }
    StreamType& operator<<(unsigned char c) {
        return operator<<(static_cast<unsigned int>(c));
    }

    StreamType& operator<<(int i) {
        char buf[64];
        buf[itoa(i, buf)] = '\0';
        print(buf);
        return *static_cast<StreamType*>(this);
    }
    StreamType& operator<<(short s) {
        return operator<<(static_cast<int>(s));
    }
    StreamType& operator<<(long l) {
        return operator<<(static_cast<int>(l));
    }

    StreamType& operator<<(unsigned int u) {
        char buf[64];
        buf[utoa(u, buf)] = '\0';
        print(buf);
        return *static_cast<StreamType*>(this);
    }
    StreamType& operator<<(unsigned short s) {
        return operator<<(static_cast<unsigned int>(s));
    }
    StreamType& operator<<(unsigned long l) {
        return operator<<(static_cast<unsigned int>(l));
    }

    StreamType& operator<<(long long int u) {
        char buf[64];
        buf[llitoa(u, buf)] = '\0';
        print(buf);
        return *static_cast<StreamType*>(this);
    }

    StreamType& operator<<(unsigned long long int u) {
        char buf[64];
        buf[llutoa(u, buf)] = '\0';
        print(buf);
        return *static_cast<StreamType*>(this);
    }

    StreamType& operator<<(const void * p) {
        char buf[64];
        buf[ptoa(p, buf)] = '\0';
        print(buf);
        return *static_cast<StreamType*>(this);
    }

    StreamType& operator<<(const char * s) {
        print(s);
        return *static_cast<StreamType*>(this);
    }

    StreamType& operator<<(float f) {
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
        return *static_cast<StreamType*>(this);
    }

protected:
    int itoa(int v, char * s)
    {
        unsigned int i = 0;

        if(v < 0) {
            v = -v;
            s[i++] = '-';
        }

        return utoa(static_cast<unsigned int>(v), s, i);
    }

    int utoa(unsigned int v, char * s, unsigned int i = 0)
    {
        unsigned int j;

        if(!v) {
            s[i++] = '0';
            return i;
        }

        if(v > 256) {
            if(_base == 8 || _base == 16)
                s[i++] = '0';
            if(_base == 16)
                s[i++] = 'x';
        }

        for(j = v; j != 0; i++, j /= _base);
        for(j = 0; v != 0; j++, v /= _base)
            s[i - 1 - j] = _digits[v % _base];

        return i;
    }

    int llitoa(long long int v, char * s)
    {
        unsigned int i = 0;

        if(v < 0) {
            v = -v;
            s[i++] = '-';
        }

        return llutoa(static_cast<unsigned long long int>(v), s, i);
    }

    int llutoa(unsigned long long int v, char * s, unsigned int i = 0)
    {
        unsigned long long int j;

        if(!v) {
            s[i++] = '0';
            return i;
        }

        if(v > 256) {
            if(_base == 8 || _base == 16)
                s[i++] = '0';
            if(_base == 16)
                s[i++] = 'x';
        }

        for(j = v; j != 0; i++, j /= _base);
        for(j = 0; v != 0; j++, v /= _base)
            s[i - 1 - j] = _digits[v % _base];

        return i;
    }

    int ptoa(const void * p, char * s)
    {
        unsigned int j, v = reinterpret_cast<unsigned int>(p);

        s[0] = '0';
        s[1] = 'x';

        for(j = 0; j < sizeof(void *) * 2; j++, v >>= 4)
            s[2 + sizeof(void *) * 2 - 1 - j]
                = _digits[v & 0xf];

        return j + 2;
    }

    // https://stackoverflow.com/questions/34222703/how-to-override-static-method-of-template-class-in-derived-class
    inline void print(const char * s) { StreamType::print(static_cast<StreamType*>(this), s); }
    inline void _set_base(int v) { _base = v; }

    int _base;
    static const char _digits[];
};

// Class Attributes
// https://stackoverflow.com/questions/3531060/how-to-initialize-a-static-const-member-in-c
template<class OStream>
const char OStream_Base<OStream>::_digits[] = "0123456789abcdef";


// https://stackoverflow.com/questions/11761506/inheritance-function-that-returns-self-type
class OStream : public OStream_Base<OStream>
{
public:
    struct Err {};

public:
    OStream(): _error(false) {}

    // Implemented on `ostream.cc`, because `stringstream.h` includes `ostream.h`,
    // hence, `ostream.h` cannot include `stringstream.h` back (cyclic reference)
    OStream & operator<<(const StringStream * stream);

    OStream & operator<<(const Begl & begl) {
        if(Traits<System>::multicore)
            _print_preamble();
        return *this;
    }

    OStream & operator<<(const Endl & endl) {
        if(Traits<System>::multicore)
            _print_trailler(_error);
        OStream_Base<OStream>::print("\n");
        _set_base(10);
        return *this;
    }

    OStream & operator<<(const Err & err) {
        _error = true;
        return *this;
    }

    using OStream_Base<OStream>::operator<<;

    // Adding virtual to the print() function caused the hysterically_debugged mode completely break
    static void print(OStream* that, const char * s) { _print(s); }

private:
    volatile bool _error;
};

extern OStream::Begl begl;
extern OStream::Endl endl;
extern OStream::Hex hex;
extern OStream::Dec dec;
extern OStream::Oct oct;
extern OStream::Bin bin;

__END_UTIL

__BEGIN_SYS
extern OStream kout, kerr;
__END_SYS

#endif
