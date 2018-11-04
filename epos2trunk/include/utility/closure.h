// EPOS Guard Component Declarations

#ifndef __closure_h
#define __closure_h

#include <utility/list.h>

__BEGIN_UTIL

class Closure_Base
{
    /// The Guard class requires access to our the _link private attribute
    friend class Guard;
    typedef List_Elements::Singly_Linked<Closure_Base> Element;

public:
    Closure_Base(): _link(this) {
        db<Synchronizer>(TRC) << "Closure_Base(_link=" << &_link << ") => " << this << endl;
    }

    /// It is not required to have a destructor for this class, then, it is not required for it to
    /// be virtual. But, we add a virtual destructor just for academic purposes of understanding.
    ~Closure_Base() {
        db<Synchronizer>(TRC) << "~Closure_Base(this=" << this << " _link=" << &_link << ")" << endl;
    }

    /// This must to be virtual otherwise the derived classes objects run() method would not be
    /// called when accessed by a base class pointer.
    virtual void run() = 0;

private:
    // Inspired by the thread code
    Element _link;
};

// https://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer/7858971
template<int ...>
struct MetaSequenceOfIntegers { };

template<int AccumulatedSize, typename Tn, int... GeneratedSequence>
struct GeneratorOfIntegerSequence;

template<int AccumulatedSize, typename Grouper, typename Head, typename... Tail, int... GeneratedSequence>
struct GeneratorOfIntegerSequence< AccumulatedSize, Grouper( Head, Tail... ), GeneratedSequence... >
{
    typedef typename GeneratorOfIntegerSequence
            < AccumulatedSize + sizeof(Head), Grouper( Tail... ), GeneratedSequence..., AccumulatedSize
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
class Closure<ReturnType( Tn... )>: public Closure_Base
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
        db<Synchronizer>(TRC) << "Closure::Closure(_entry=" << &_entry << ", PARAMETERS_COUNT=" << PARAMETERS_COUNT
                << ", PARAMETERS_LENGTH=" << PARAMETERS_LENGTH << ", sizeof=" << sizeof(*this) << ") => " << this << endl;

        _parameters = new char[PARAMETERS_LENGTH];
        pack_helper( _parameters, an ... );
    }

    ~Closure() {
        db<Synchronizer>(TRC) << "Closure::Closure(this=" << this << "_entry=" << &_entry << ", PARAMETERS_COUNT=" << PARAMETERS_COUNT
                << ", PARAMETERS_LENGTH=" << PARAMETERS_LENGTH << ", sizeof=" << sizeof(*this) << ")" << endl;

        delete _parameters;
    }

    ReturnType run() {
        return _run( typename GeneratorOfIntegerSequence< 0, int(Tn...) >::type() );
    }

private:
    template<int ...Sequence>
    ReturnType _run(MetaSequenceOfIntegers<Sequence...>)
    {
        db<Synchronizer>(TRC) << "Closure::_run(this=" << this << ")" << endl;
        return _entry( unpack_helper<Sequence, Tn>()... );
    }

    template<const int position, typename T>
    T unpack_helper()
    {
        db<Synchronizer>(TRC) << "Closure::unpack_helper(Head=" << sizeof( T )
                << ", address=" << reinterpret_cast<int *>(_parameters + position)
                << "(" << reinterpret_cast<int *>(_parameters) << ")" 
                << ", position=" << position << ")" << endl;

        return *reinterpret_cast<T *>( _parameters + position );
    }

public:
    template<typename Head, typename ... Tail>
    static void pack_helper(char* pointer_address, Head head, Tail ... tail)
    {
        db<Synchronizer>(TRC) << "Closure::pack_helper(Head=" << sizeof( Head )
                << ", address=" << reinterpret_cast<int *>(pointer_address) << ")" << endl;

        *reinterpret_cast<Head *>(pointer_address) = head;
        pack_helper(pointer_address + sizeof( Head ), tail ...);
    }

    static void pack_helper(char* pointer_address) {}
};

__END_UTIL

#endif
