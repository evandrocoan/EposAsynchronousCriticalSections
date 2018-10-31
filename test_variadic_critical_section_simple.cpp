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

void test_function(char arg1, int arg2)
{
    printf("Running test_function\n");
}

int main()
{
    new Critical_Section<char, int>(&test_function, 'a', 1);
}

