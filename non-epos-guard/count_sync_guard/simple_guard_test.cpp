#include "guard.h"

void example() {
    DB( "Example begin!" << std::endl )
}

// g++ -o test $APPLICATION -lpthread -m32 && ./test
// sudo chrt --rr 99 ./guarded_count &> test_guarded.log
int main() {
    DB( "Main begin!" << std::endl )
    Guard guard;

    guard.submit(example);
}
