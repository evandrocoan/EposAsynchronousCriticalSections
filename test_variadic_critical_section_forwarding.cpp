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
    typedef ReturnType(*Function)(Tn ...);
    Function _entry;
    char* _parameters;

    Closure(Function _entry, Tn ... an): _entry(_entry)
    {
        static const unsigned int PARAMETERS_SIZE = SIZEOF<Tn ...>::Result;
        printf( "Running Closure::Closure(this=%d, _entry=%d, _size=%d)\n", this, &_entry, PARAMETERS_SIZE );

        _parameters = new char[PARAMETERS_SIZE];
        pack_helper(_parameters, an ...);
    }

    ReturnType operator()()
    {
        printf( "Running operator(this=%d)\n", this );
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

/**
 * Create a closure which can have any return type, except void because:
 *  1. The function caller which runs internally the closure, returns something.
 *  2. The `ArgumentEvaluteOrderer` also returns something while guarantee the function parameters
 *     correct evaluation order.
 */
template<typename ReturnType, typename ... Tn>
Closure<ReturnType, Tn ...> create_closure( ReturnType(*_entry)( Tn ... ), Tn ... an )
{
    auto closure = new Closure<ReturnType, Tn ...>( _entry, an ... );
    printf( "Running create_closure: %d\n", closure );
    return *closure;
}

char test_function1(char arg1, int arg2, bool arg3) {
    printf("   test_function: %c, %d, %d\n", arg1, arg2, arg3);
}

char test_function2(const char* arg1, const char* arg2, char arg3) {
    printf("   test_function: %s, %s, %c\n", arg1, arg2, arg3);
}

// clang++ -Xclang -ast-print -fsyntax-only test.cpp
// https://stackoverflow.com/questions/4448094/can-we-see-the-template-instantiated-code-by-c-compiler
int main()
{
    auto my_closure1 = create_closure( &test_function1, 'a', 1, true ); printf("\n");
    auto my_closure2 = create_closure( &test_function2, "testa 1", "testa 2", 'a' ); printf("\n");

    my_closure1(); printf("\n");
    my_closure2();
}

// References
// https://stackoverflow.com/questions/18847424/c-create-custom-function-dispatcher-from-variadic-template
// https://stackoverflow.com/questions/42047795/order-of-parameter-pack-expansion
// https://stackoverflow.com/questions/29194858/order-of-function-calls-in-variadic-template-expansion
// https://stackoverflow.com/questions/43553585/how-to-transform-a-variadic-template-argument-to-another-types-for-calling-anoth
// https://stackoverflow.com/questions/12030538/calling-a-function-for-each-variadic-template-argument-and-an-array
// https://stackoverflow.com/questions/687490/how-do-i-expand-a-tuple-into-variadic-template-functions-arguments/12650100#12650100
// https://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer
// https://en.cppreference.com/w/cpp/language/list_initialization#Notes
// https://stackoverflow.com/questions/47207621/build-function-parameters-with-variadic-templates
// https://stackoverflow.com/questions/16868129/how-to-store-variadic-template-arguments
// https://stackoverflow.com/questions/43026550/how-to-define-a-class-that-can-save-variadic-template-arguments
// https://stackoverflow.com/questions/17996003/how-to-save-variable-number-of-arguments-using-variadic-template-arguments
// https://stackoverflow.com/questions/29220248/is-it-possible-to-store-variadic-arguments-into-a-member-variable
// https://stackoverflow.com/questions/4691657/is-it-possible-to-store-a-template-parameter-pack-without-expanding-it
// https://arne-mertz.de/2016/11/modern-c-features-variadic-templates/
// https://gitlab.com/evandro-crr/epos2/tree/master
// https://en.wikipedia.org/wiki/Rate-monotonic_scheduling
// https://pdfs.semanticscholar.org/b16e/8dd03e52e47296c4658520bf54d13040484e.pdf
// https://stackoverflow.com/questions/13108663/storing-functions-call-and-list-of-parameters-to-invoke-later
// https://stackoverflow.com/questions/4926433/saving-function-pointerarguments-for-later-use
// https://appuals.com/fix-cannot-execute-binary-file-exec-format-error-ubuntu/
// https://stackoverflow.com/questions/30926764/extracting-function-argument-types-as-a-parameter-pack
// https://stackoverflow.com/questions/24948277/unpacking-arguments-of-a-functional-parameter-to-a-c-template-class
// https://stackoverflow.com/questions/14783876/c-is-it-possible-to-extract-class-and-argument-types-from-a-member-function
// https://stackoverflow.com/questions/34836104/how-to-extract-a-selected-set-of-arguments-of-a-variadic-function-and-use-them-t
// https://stackoverflow.com/questions/28033251/can-you-extract-types-from-template-parameter-function-signature
// https://stackoverflow.com/questions/11056714/c-type-traits-to-extract-template-parameter-class
// https://stackoverflow.com/questions/32674839/variadic-member-function-of-template-class
// https://stackoverflow.com/questions/49217891/class-member-function-in-variadic-template
// https://stackoverflow.com/questions/50316284/how-to-achieve-variadic-virtual-member-function
// https://stackoverflow.com/questions/15599679/class-member-function-pointer
// https://stackoverflow.com/questions/40855835/how-do-you-typedef-a-function-pointer-type-with-parameter-pack-arguments
// https://stackoverflow.com/questions/8915797/calling-a-function-through-its-address-in-memory-in-c-c
// https://stackoverflow.com/questions/47005664/call-function-by-known-address-c
// https://en.cppreference.com/w/cpp/language/parameter_pack
// https://stackoverflow.com/questions/47037395/forward-types-in-variadic-template-as-values-references-according-to-function-si
// https://eli.thegreenplace.net/2014/perfect-forwarding-and-universal-references-in-c
// https://stackoverflow.com/questions/3836648/structure-or-class-with-variable-number-of-members
// https://stackoverflow.com/questions/43069213/c-class-template-for-automatic-getter-setter-methods-good-bad-practice
// https://stackoverflow.com/questions/13980157/c-class-with-template-member-variable
// https://stackoverflow.com/questions/6261375/how-to-declare-data-members-that-are-objects-of-any-type-in-a-class
// https://stackoverflow.com/questions/1872220/is-it-possible-to-iterate-over-arguments-in-variadic-macros
// https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments
// https://groups.google.com/forum/#!topic/comp.std.c/d-6Mj5Lko_s
// https://stackoverflow.com/questions/27941661/generating-one-class-member-per-variadic-template-argument
// https://stackoverflow.com/questions/14919990/c-how-to-template-a-class-attribute-and-not-the-functions-of-the-class
// https://stackoverflow.com/questions/40204338/template-parameter-pack-attribute
// https://stackoverflow.com/questions/5723619/is-it-possible-to-create-function-local-closures-pre-c11
// https://blog.feabhas.com/2014/03/demystifying-c-lambdas/
// https://stackoverflow.com/questions/1447199/c-closures-and-templates
// http://matt.might.net/articles/c++-template-meta-programming-with-lambda-calculus/
// https://www.gnu.org/software/gcc/gcc-4.4/cxx0x_status.html
//

