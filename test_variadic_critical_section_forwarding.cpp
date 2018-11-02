#include <stdio.h>

template<typename ... Tn>
class Closure
{
public:
    typedef void (*Function)(Tn ...);
    Function _entry;

    Closure(Function _entry, Tn ... an): _entry(_entry)
    {
        printf( "Running Closure\n" );
    }

    void operator()() 
    {
        this->_entry( 'b', 2 );
    }
};

void test_function(char arg1, int arg2)
{
    printf("Running test_function\n");
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

