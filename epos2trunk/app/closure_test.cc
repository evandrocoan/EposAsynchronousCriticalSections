// EPOS Synchronizer Component Test Program

#include <utility/closure.h>

using namespace EPOS;

// #include <semaphore.h>
// Semaphore display_lock;
// #define log(argument) display_lock.p(); db<Synchronizer>(WRN) << argument; display_lock.v();
#define log(argument) db<Synchronizer>(WRN) << argument;

template<typename ReturnType, typename ... Tn>
Closure< ReturnType(Tn ...) > create_closure( ReturnType(*_entry)( Tn ... ), Tn ... an )
{
    auto closure = new Closure< ReturnType(Tn ...) >( _entry, an ... );
    log( "create_closure " << closure << endl )
    return *closure;
}

template<typename ... Tn>
void test_closure(Tn ... an) {
    auto closure = create_closure(an ...);
    closure();
    log( "    void" << endl << endl )
}

template<typename ... Tn>
void test_closure_with_return(Tn ... an) {
    auto closure = create_closure(an ...);
    log( "    " << closure() << endl << endl )
}

char test_function1(char arg1, int arg2, bool arg3) {
    log( "test_function1 " << arg1 << ", " <<  arg2 << ", " << arg3 << endl )
    return 'A';
}

char test_function2(const char* arg1, const char* arg2, char arg3) {
    log( "test_function2 " << arg1 << ", " <<  arg2 << ", " << arg3 << endl )
    return 'B';
}

char test_function3() {
    log( "test_function3 " << endl )
    return 'C';
}

void test_function4() {
    log( "test_function4 " << endl )
}

void test_function5(const char* arg1) {
    log( "test_function5 " << arg1 << endl )
}

int main()
{
    log( endl << "main: begin()" << endl )

    test_closure_with_return( &test_function1, 'a', 10, false );
    test_closure_with_return( &test_function2, "test1", "test2", 'b' );
    test_closure_with_return( &test_function3 );

    test_closure( &test_function4 );
    test_closure( &test_function5, "Testa 3" );
    test_closure( &test_function5, "Testa 4" );

    log( "main: exiting()" << endl )
    return 0;
}
