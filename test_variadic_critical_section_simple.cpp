#include <stdio.h>

template<typename ... Tn>
class Closure
{
public:
    typedef void (*Function)(Tn ...);
    Function _entry;

    Closure(Function _entry, Tn ... an): _entry(_entry)
    {
        printf("Running Closure\n");
        this->_entry('b', 2);
    }
};

void test_function(char arg1, int arg2)
{
    printf("Running test_function\n");
}

int main()
{
    new Closure<char, int>(&test_function, 'a', 1);
}

