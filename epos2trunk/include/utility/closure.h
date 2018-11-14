// EPOS Guard Component Declarations

#include <utility/debug_sync.h>

#ifndef __closure_h
#define __closure_h

__BEGIN_UTIL

// https://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer/7858971
template<int ...>
struct MetaSequenceOfIntegers { };

template<int AccumulatedSize, typename Tn, int... GeneratedSequence>
struct GeneratorOfIntegerSequence;

template<
            int AccumulatedSize,
            typename Grouper,
            typename Head,
            typename... Tail,
            int... GeneratedSequence
        >
struct GeneratorOfIntegerSequence<
        AccumulatedSize, Grouper( Head, Tail... ), GeneratedSequence... >
{
    typedef typename GeneratorOfIntegerSequence
            <
                AccumulatedSize + sizeof(Head),
                Grouper( Tail... ),
                GeneratedSequence...,
                AccumulatedSize
            >::type type;
};

template<int AccumulatedSize, typename Grouper, int... GeneratedSequence>
struct GeneratorOfIntegerSequence<AccumulatedSize, Grouper(), GeneratedSequence...>
{
  typedef MetaSequenceOfIntegers<GeneratedSequence...> type;
};

// https://stackoverflow.com/questions/34957810/variadic-templates-parameter-pack-and-its-discussed-ambiguity-in-a-parameter-li
template<typename Tn>
class Closure;

template<typename ReturnType, typename... Tn>
class Closure<ReturnType( Tn... )>
{
public:
    typedef ReturnType(*Function)(Tn ...);
    static const unsigned int PARAMETERS_COUNT = sizeof...( Tn );
    static const unsigned int PARAMETERS_LENGTH = SIZEOF<Tn ...>::Result;

private:
    Function _entry;
    char* _parameters;

public:
    Closure(Function _entry, Tn ... an): _entry(_entry)
    {
        LOG( Closure, TRC, "Closure(_entry=" << &_entry
                << ", PARAMETERS_COUNT=" << PARAMETERS_COUNT
                << ", PARAMETERS_LENGTH=" << PARAMETERS_LENGTH
                << ", sizeof=" << sizeof(*this) << ") => " << this << endl )

        if(PARAMETERS_LENGTH)
            _parameters = new char[PARAMETERS_LENGTH];

        pack_helper( _parameters, an ... );
    }

    ~Closure() {
        LOG( Closure, TRC, "~Closure(this=" << this
                << ", _entry=" << &_entry << ", PARAMETERS_COUNT=" << PARAMETERS_COUNT
                << ", PARAMETERS_LENGTH=" << PARAMETERS_LENGTH
                << ", sizeof=" << sizeof(*this) << ")" << endl )

        if(PARAMETERS_LENGTH)
            delete _parameters;
    }

    inline ReturnType run() {
        return operator()();
    }

    inline ReturnType operator()() {
        return _unpack_and_run( typename GeneratorOfIntegerSequence< 0, int(Tn...) >::type() );
    }

private:
    template<int ...Sequence>
    inline ReturnType _unpack_and_run(MetaSequenceOfIntegers<Sequence...>)
    {
        LOG( Closure, TRC, "Closure::_unpack_and_run(this=" << this << ")" << endl )
        return _entry( unpack_helper<Sequence, Tn>()... );
    }

    template<const int position, typename T>
    inline T unpack_helper()
    {
        LOG( Closure, TRC, "Closure::unpack_helper(Head=" << sizeof( T )
                << ", address=" << reinterpret_cast<int *>(_parameters + position)
                << "(" << reinterpret_cast<int *>(_parameters) << ")"
                << ", position=" << position << ")" << endl )

        return *reinterpret_cast<T *>( _parameters + position );
    }

public:
    template<typename Head, typename ... Tail>
    static void pack_helper(char* pointer_address, Head head, Tail ... tail)
    {
        LOG( Closure, TRC, "Closure::pack_helper(Head=" << sizeof( Head )
                << ", address=" << reinterpret_cast<int *>(pointer_address) << ")" << endl )

        *reinterpret_cast<Head *>(pointer_address) = head;
        pack_helper(pointer_address + sizeof( Head ), tail ...);
    }

    static void pack_helper(char* pointer_address) {}
};

__END_UTIL

#endif
