#include <stdio.h>

// SIZEOF Type Package
template<typename ... Tn>
struct SIZEOF
{ static const unsigned int Result = 0; };

template<typename T1, typename ... Tn>
struct SIZEOF<T1, Tn ...>
{ static const unsigned int Result = sizeof(T1) + SIZEOF<Tn ...>::Result ; };

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
        printf( "Closure::Closure(_entry=%d, PARAMETERS_COUNT=%d, PARAMETERS_LENGTH=%d, sizeof=%d) => %d\n",
                &_entry, PARAMETERS_COUNT, PARAMETERS_LENGTH, sizeof(*this), this );

        if(PARAMETERS_LENGTH) _parameters = new char[PARAMETERS_LENGTH];
        pack_helper( _parameters, an ... );
    }

    ~Closure() {
        printf( "Closure::~Closure(this=%d, _entry=%d, PARAMETERS_COUNT=%d, PARAMETERS_LENGTH=%d, sizeof=%d)\n",
                this, &_entry, PARAMETERS_COUNT, PARAMETERS_LENGTH, sizeof(*this) );

        if(PARAMETERS_LENGTH) delete _parameters;
    }

    ReturnType operator()() {
        return _run( typename GeneratorOfIntegerSequence< 0, int(Tn...) >::type() );
    }

private:
    template<int ...Sequence>
    ReturnType _run(MetaSequenceOfIntegers<Sequence...>)
    {
        printf( "Closure::_run(this=%d)\n", this );
        return _entry( unpack_helper<Sequence, Tn>()... );
    }

    template<const int position, typename T>
    T unpack_helper()
    {
        printf( "Closure::unpack_helper(Head=%d, address=%d(%d), position=%d)\n",
                sizeof( T ), _parameters + position, _parameters, position );

        return *reinterpret_cast<T *>( _parameters + position );
    }

public:
    template<typename Head, typename ... Tail>
    static void pack_helper(char* pointer_address, Head head, Tail ... tail)
    {
        printf( "Closure::pack_helper(Head=%d, address=%d)\n", sizeof( Head ), pointer_address );

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
    printf( "create_closure=%d\n", closure );
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

void test_function5(const char* arg1) {
    printf("   test_function5=%s\n", arg1);
}

template<typename ... Tn>
void test_closure(Tn ... an) {
    auto closure = create_closure(an ...);
    closure();
    printf( "\n" );
}

// clang++ -Xclang -ast-print -fsyntax-only > test_variadic_critical_section_expanded.cpp
// https://stackoverflow.com/questions/4448094/can-we-see-the-template-instantiated-code-by-c-compiler
int main()
{
    test_closure( &test_function1, 'a', 10, false );
    test_closure( &test_function2, "test1", "test2", 'b' );
    test_closure( &test_function3 );
    test_closure( &test_function4 );
    test_closure( &test_function5, "Testa 3" );
    test_closure( &test_function5, "Testa 4" );
}

