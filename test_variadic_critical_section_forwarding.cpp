#include <stdio.h>

template<typename ... Tn>
class Critical_Section
{
public:
    typedef void (*Function)(Tn ...);
    Function _entry;

    Critical_Section(Function _entry, Tn ... an): _entry(_entry)
    {
        printf("Running Critical_Section\n");
        this->_entry('b', 2);
    }
};

void test_function2(char arg1, int arg2)
{
    printf("Running test_function2\n");
}

template<typename ... Tn>
void test_function1(Tn ... an)
{
    printf("Running test_function1\n");
    new Critical_Section<Tn ...>(&test_function2, an ...);
}

int main()
{
    test_function1('a', 1);
}

