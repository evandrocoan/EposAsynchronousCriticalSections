#include <stdio.h>

// SIZEOF Type Package
template<typename ... Tn>
struct SIZEOF
{ static const unsigned int  Result = 0; };

template<typename T1, typename ... Tn>
struct SIZEOF<T1, Tn ...>
{ static const unsigned int Result = sizeof(T1) + SIZEOF<Tn ...>::Result ; };

template<typename ... Tn>
class Closure
{
public:
    typedef void (*Function)(Tn ...);
    Function _entry;
    char* _parameters;

    Closure(Function _entry, Tn ... an): _entry(_entry)
    {
        static const unsigned int PARAMETERS_SIZE = SIZEOF<Tn ...>::Result;
        _parameters = new char[PARAMETERS_SIZE];

        printf( "Running Closure::Closure, size: %d, address: %d\n", PARAMETERS_SIZE, _parameters );
        init_helper(_parameters, an ...);
    }

    void operator()() 
    {
        this->_entry( 'b', 2 );
    }

    template<typename Head, typename ... Tail>
    static void init_helper(char* pointer_address, Head head, Tail ... tail) 
    {
        printf( "Running Closure::init_helper, Head: %d\n", sizeof(Head) );
        *reinterpret_cast<Head *>(pointer_address) = head;
        init_helper(pointer_address + sizeof(Head), tail ...);
    }

    static void init_helper(char* pointer_address) {}
};

void test_function(char arg1, int arg2)
{
    printf("Running test_function, arg1: %c, arg2: %d\n", arg1, arg2);
}

template<typename ... Tn>
Closure<Tn ...> create_closure(void(*_entry)(Tn ...), Tn ... an)
{
    printf( "Running create_closure\n" );
    return *( new Closure<Tn ...>( _entry, an ... ) );
}

int main()
{
    auto my_closure = create_closure( &test_function, 'a', 1 );
    my_closure();
}

