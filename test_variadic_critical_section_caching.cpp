#include <stdio.h>

// SIZEOF Type Package
template<typename ... Tn>
struct SIZEOF
{ static const unsigned int Result = 0; };

template<typename T1, typename ... Tn>
struct SIZEOF<T1, Tn ...>
{ static const unsigned int Result = sizeof(T1) + SIZEOF<Tn ...>::Result ; };

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
class Closure< ReturnType( Tn... ) >
{
public:
    typedef ReturnType(*Function)(Tn ...);
    static constexpr unsigned int parameters_count = sizeof...( Tn );

    Function _entry;
    char* _parameters;

    Closure(Function _entry, Tn ... an): _entry(_entry)
    {
        static const unsigned int PARAMETERS_SIZE = SIZEOF<Tn ...>::Result;
        printf( "Running Closure::Closure(this=%d, _entry=%d, _size=%d)\n", this, &_entry, PARAMETERS_SIZE );

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

    ReturnType operator()()
    {
        return _run( typename GeneratorOfIntegerSequence< sizeof...(Tn) + 1 >::type() );
    }

    template<int ...Sequence>
    ReturnType _run(MetaSequenceOfIntegers<Sequence...>)
    {
        printf( "Running operator(this=%d)\n", this );

        char* walker = _parameters;
        return _entry( unpack_helper<Sequence, Tn>(walker)... );
    }

    template<typename Head, typename ... Tail>
    static void caller_helper(char* pointer_address, Head head, Tail ... tail)
    {
        constexpr int count = parameters_count - sizeof...( Tail );
        printf( "Running Closure::caller_helper, Head: %d, address: %d, count: %d\n", sizeof( Head ), pointer_address, count );

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
            printf( "Running Closure::unpack_helper, Head: %d, Getting cached value...\n", sizeof( T ) );
            return real_value;
        }

        printf( "Running Closure::unpack_helper, Head: %d, address: %d, count: %d\n", sizeof( T ), pointer_address, count );
        char* old = pointer_address;
        pointer_address += sizeof( T );

        is_defined = true;
        real_value = *reinterpret_cast<T *>( old );
        return real_value;
    }

    template<typename Head, typename ... Tail>
    static void pack_helper(char* pointer_address, Head head, Tail ... tail)
    {
        printf( "Running Closure::pack_helper, Head: %d, address: %d\n", sizeof( Head ), pointer_address );

        *reinterpret_cast<Head *>(pointer_address) = head;
        pack_helper(pointer_address + sizeof( Head ), tail ...);
    }

    static void pack_helper(char* pointer_address) {}
};

/**
 * Create a closure which can have any return type.
 */
template<typename ReturnType, typename ... Tn>
Closure< ReturnType(Tn ...) > create_closure( ReturnType(*_entry)( Tn ... ), Tn ... an )
{
    auto closure = new Closure< ReturnType(Tn ...) >( _entry, an ... );
    printf( "Running create_closure: %d\n", closure );
    return *closure;
}

char test_function1(char arg1, int arg2, bool arg3) {
    printf("   test_function1: %c, %d, %d\n", arg1, arg2, arg3);
}

char test_function2(const char* arg1, const char* arg2, char arg3) {
    printf("   test_function2: %s, %s, %c\n", arg1, arg2, arg3);
}

char test_function3() {
    printf("   test_function3\n");
}

void test_function4() {
    printf("   test_function4\n");
}

// clang++ -Xclang -ast-print -fsyntax-only > test_variadic_critical_section_expanded.cpp
// https://stackoverflow.com/questions/4448094/can-we-see-the-template-instantiated-code-by-c-compiler
int main()
{
    auto my_closure1 = create_closure( &test_function1, 'a', 10, false ); printf("\n");
    auto my_closure2 = create_closure( &test_function2, "testa 1", "testa 2", 'a' ); printf("\n");
    auto my_closure3 = create_closure( &test_function3 ); printf("\n");
    auto my_closure4 = create_closure( &test_function4 ); printf("\n");

    my_closure1(); printf("\n");
    my_closure2(); printf("\n");
    my_closure3(); printf("\n");
    my_closure4(); printf("\n");
}

