#include "guard.h"

void example() {
    DB( "Example begin!" << std::endl )
}

// g++ -o test simple_guard_test.cpp -lpthread -m32 -ggdb && ./test
int main() {
    DB( "Main begin!" << std::endl )
    Guard guard;

    guard.submit(example);
}
