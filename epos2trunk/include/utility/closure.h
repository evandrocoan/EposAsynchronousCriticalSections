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

// https://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer/7858971#7858971
template<int ...>
struct MetaSequenceOfIntegers { };

template<int CurrentCount, int ...GeneratedSequence>
struct GeneratorOfIntegerSequence: GeneratorOfIntegerSequence<CurrentCount-1, CurrentCount-1, GeneratedSequence...> { };

template<int ...GeneratedSequence>
struct GeneratorOfIntegerSequence<1, GeneratedSequence...>
{
  typedef MetaSequenceOfIntegers<GeneratedSequence...> type;
};

// https://stackoverflow.com/questions/34957810/variadic-templates-parameter-pack-and-its-discussed-ambiguity-in-a-parameter-li
template < typename Tn >
class Closure;

template < typename ReturnType, typename... Tn >
class Closure< ReturnType( Tn... ) >: public Closure_Base
{
public:
    typedef ReturnType(*Function)(Tn ...);
    static const unsigned int parameters_count = sizeof...( Tn );
    static const unsigned int PARAMETERS_SIZE = SIZEOF<Tn ...>::Result;

    Closure(Function _entry, Tn ... an): _entry(_entry)
    {
        db<Synchronizer>(TRC) << "Closure(_entry=" << &_entry << ", _size=" << PARAMETERS_SIZE
                << ", sizeof=" << sizeof(*this) << ") => " << this << endl;

        _parameters = new char[PARAMETERS_SIZE];
        pack_helper( _parameters, an ... );

        /// A recursion to force the function parameters to be cached locally on each function,
        /// because the initializer list on GCC 4.4.4 is not working, only on GCC 7.2.0.
        ///
        /// This bug was only fixed on GCC 4.9.1 - https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51253
        /// Bug 51253 - [C++11][DR 1030] Evaluation order (sequenced-before relation) among initializer-clauses in braced-init-list
        char* walker = _parameters;
        caller_helper( walker, an ... );
    }

    ~Closure() {
        db<Synchronizer>(TRC) << "Running Closure::~Closure(this=" << this << ", _entry=" << &_entry
                << ", _size=" << PARAMETERS_SIZE << ", address=" <<  reinterpret_cast<int *>(_parameters) << ")" << endl;

        delete _parameters;
    }

    ReturnType run() {
        return _run( typename GeneratorOfIntegerSequence< sizeof...(Tn) + 1 >::type() );
    }

private:
    template<int ...Sequence>
    ReturnType _run(MetaSequenceOfIntegers<Sequence...>)
    {
        db<Synchronizer>(TRC) << "Running operator(this=" << this << ")" << endl;

        char* walker = _parameters;
        return _entry( unpack_helper<Sequence, Tn>(walker)... );
    }

    template<typename Head, typename ... Tail>
    static void caller_helper(char* pointer_address, Head head, Tail ... tail)
    {
        const int count = parameters_count - sizeof...( Tail );
        db<Synchronizer>(TRC) << "Running Closure::caller_helper, Head=" << sizeof( Head )
                << ", address=" << reinterpret_cast<int *>(pointer_address) << ", count=" << count << ")" << endl;

        unpack_helper<count, Head>( pointer_address );
        caller_helper( pointer_address, tail... );
    }

    static void caller_helper(char* pointer_address) {}

    template<const int count, typename T>
    static T unpack_helper(char* &pointer_address)
    {
        static T real_value;
        static bool is_defined = false;

        if( is_defined ) {
            db<Synchronizer>(TRC) << "Running Closure::unpack_helper, Head=" << sizeof( T ) << ", Getting cached value..." << endl;
            return real_value;
        }

        db<Synchronizer>(TRC) << "Running Closure::unpack_helper, Head=" << sizeof( T )
                << ", address=" << reinterpret_cast<int *>(pointer_address) << ", count=" << count << ")" << endl;

        char* old = pointer_address;
        pointer_address += sizeof( T );

        is_defined = true;
        real_value = *reinterpret_cast<T *>( old );
        return real_value;
    }

public:
    template<typename Head, typename ... Tail>
    static void pack_helper(char* pointer_address, Head head, Tail ... tail)
    {
        db<Synchronizer>(TRC) << "Closure::pack_helper, Head=" << sizeof( Head )
                << ", address=" << reinterpret_cast<int *>(pointer_address) << ", value=" << head << endl;

        *reinterpret_cast<Head *>(pointer_address) = head;
        pack_helper(pointer_address + sizeof( Head ), tail ...);
    }

    static void pack_helper(char* pointer_address) {}

private:
    Function _entry;
    char* _parameters;
};

__END_UTIL

#endif
