#include <stdio.h>

// SIZEOF Type Package
template<typename ... Tn>
struct SIZEOF
{ static const unsigned int Result = 0; };

template<typename T1, typename ... Tn>
struct SIZEOF<T1, Tn ...>
{ static const unsigned int Result = sizeof(T1) + SIZEOF<Tn ...>::Result ; };

/// https://stackoverflow.com/questions/47207621/build-function-parameters-with-variadic-templates
template<class ReturnType>
struct ArgumentEvaluteOrderer
{
    /// This forbids our function from having a void return type
    ReturnType return_value;

    template<class... Args>
    ArgumentEvaluteOrderer( ReturnType(*function)( Args... ), Args... args ): return_value{ function( args... ) }
    {
    }

    operator ReturnType() const { return return_value; }
};

template<typename ReturnType, typename ... Tn>
class Closure
{
public:
    typedef ReturnType (*Function)(Tn ...);
    Function _entry;
    char* _parameters;

    Closure(Function _entry, Tn ... an): _entry(_entry)
    {
        static const unsigned int PARAMETERS_SIZE = SIZEOF<Tn ...>::Result;
        _parameters = new char[PARAMETERS_SIZE];

        printf( "Running Closure::Closure, size: %d, address: %d\n", PARAMETERS_SIZE, _parameters );
        pack_helper(_parameters, an ...);
    }

    ReturnType operator()()
    {
        char* walker = _parameters;
        return ArgumentEvaluteOrderer<ReturnType>{ _entry, unpack_helper<Tn>(walker)... };
    }

    template<typename T>
    static T unpack_helper(char* &_parameters)
    {
        printf( "Running unpack_helper T: %d, address: %d\n", sizeof( T ), _parameters );
        char* old = _parameters;

        _parameters += sizeof( T );
        return *reinterpret_cast<T *>( old );
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

char test_function(char arg1, int arg2, bool arg3)
{
    printf("Running test_function, arg1: %c, arg2: %d, arg3: %d\n", arg1, arg2, arg3);
}

/**
 * Create a closure which can have any return type, except void because:
 *  1. The function caller which runs internally the closure, returns something.
 *  2. The `ArgumentEvaluteOrderer` also returns something while guarantee the function parameters
 *     correct evaluation order.
 */
template<typename ReturnType, typename ... Tn>
Closure<ReturnType, Tn ...> create_closure( ReturnType(*_entry)( Tn ... ), Tn ... an )
{
    printf( "Running create_closure\n" );
    return *( new Closure<ReturnType, Tn ...>( _entry, an ... ) );
}

// clang++ -Xclang -ast-print -fsyntax-only test.cpp
// https://stackoverflow.com/questions/4448094/can-we-see-the-template-instantiated-code-by-c-compiler
int main()
{
    auto my_closure = create_closure( &test_function, 'a', 1, true );
    my_closure();
}

