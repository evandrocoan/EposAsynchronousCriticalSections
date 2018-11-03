#include <stdio.h>

// SIZEOF Type Package
template<typename ... Tn>
struct SIZEOF
{ static const unsigned int Result = 0; };

template<typename T1, typename ... Tn>
struct SIZEOF<T1, Tn ...>
{ static const unsigned int Result = sizeof(T1) + SIZEOF<Tn ...>::Result ; };

// https://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer/7858971#7858971
template<int ...>
struct MetaSequenceOfIntegers { };

template<int CurrentCount, int ...GeneratedSequence>
struct GeneratorOfIntegerSequence: GeneratorOfIntegerSequence<CurrentCount-1, CurrentCount-1, GeneratedSequence...> { };

template<int ...GeneratedSequence>
struct GeneratorOfIntegerSequence<1, GeneratedSequence...>
{
  typedef MetaSequenceOfIntegers<GeneratedSequence...> type;
};

// https://stackoverflow.com/questions/34957810/variadic-templates-parameter-pack-and-its-discussed-ambiguity-in-a-parameter-li
template < typename Tn >
class Closure;

template < typename ReturnType, typename... Tn >
class Closure< ReturnType( Tn... ) >
{
public:
    typedef ReturnType(*Function)(Tn ...);
    static constexpr unsigned int parameters_count = sizeof...( Tn );

    Function _entry;
    char* _parameters;

    Closure(Function _entry, Tn ... an): _entry(_entry)
    {
        static const unsigned int PARAMETERS_SIZE = SIZEOF<Tn ...>::Result;
        printf( "Running Closure::Closure(this=%d, _entry=%d, _size=%d)\n", this, &_entry, PARAMETERS_SIZE );

        _parameters = new char[PARAMETERS_SIZE];
        pack_helper( _parameters, an ... );

        /// A recursion to force the function parameters to be cached locally on each function,
        /// because the initializer list on GCC 4.4.4 is not working, only on GCC 7.2.0.
        ///
        /// This bug was only fixed on GCC 4.9.1 - https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51253
        /// Bug 51253 - [C++11][DR 1030] Evaluation order (sequenced-before relation) among initializer-clauses in braced-init-list
        char* walker = _parameters;
        caller_helper( walker, an ... );
    }

    ReturnType operator()()
    {
        return _run( typename GeneratorOfIntegerSequence< sizeof...(Tn) + 1 >::type() );
    }

    template<int ...Sequence>
    ReturnType _run(MetaSequenceOfIntegers<Sequence...>)
    {
        printf( "Running operator(this=%d)\n", this );

        char* walker = _parameters;
        return _entry( unpack_helper<Sequence, Tn>(walker)... );
    }

    template<typename Head, typename ... Tail>
    static void caller_helper(char* pointer_address, Head head, Tail ... tail)
    {
        constexpr int count = parameters_count - sizeof...( Tail );
        printf( "Running Closure::caller_helper, Head: %d, address: %d, count: %d\n", sizeof( Head ), pointer_address, count );

        unpack_helper<count, Head>( pointer_address );
        caller_helper( pointer_address, tail... );
    }

    static void caller_helper(char* pointer_address) {}

    template<const int count, typename T>
    static T unpack_helper(char* &pointer_address)
    {
        static T real_value;
        static bool is_defined = false;

        if( is_defined ) {
            printf( "Running Closure::unpack_helper, Head: %d, Getting cached value...\n", sizeof( T ) );
            return real_value;
        }

        printf( "Running Closure::unpack_helper, Head: %d, address: %d, count: %d\n", sizeof( T ), pointer_address, count );
        char* old = pointer_address;
        pointer_address += sizeof( T );

        is_defined = true;
        real_value = *reinterpret_cast<T *>( old );
        return real_value;
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
 * Create a closure which can have any return type.
 */
template<typename ReturnType, typename ... Tn>
Closure< ReturnType(Tn ...) > create_closure( ReturnType(*_entry)( Tn ... ), Tn ... an )
{
    auto closure = new Closure< ReturnType(Tn ...) >( _entry, an ... );
    printf( "Running create_closure: %d\n", closure );
    return *closure;
}

char test_function1(char arg1, int arg2, bool arg3) {
    printf("   test_function1: %c, %d, %d\n", arg1, arg2, arg3);
}

char test_function2(const char* arg1, const char* arg2, char arg3) {
    printf("   test_function2: %s, %s, %c\n", arg1, arg2, arg3);
}

char test_function3() {
    printf("   test_function3\n");
}

void test_function4() {
    printf("   test_function4\n");
}

// clang++ -Xclang -ast-print -fsyntax-only > test_variadic_critical_section_expanded.cpp
// https://stackoverflow.com/questions/4448094/can-we-see-the-template-instantiated-code-by-c-compiler
int main()
{
    auto my_closure1 = create_closure( &test_function1, 'a', 10, false ); printf("\n");
    auto my_closure2 = create_closure( &test_function2, "testa 1", "testa 2", 'a' ); printf("\n");
    auto my_closure3 = create_closure( &test_function3 ); printf("\n");
    auto my_closure4 = create_closure( &test_function4 ); printf("\n");

    my_closure1(); printf("\n");
    my_closure2(); printf("\n");
    my_closure3(); printf("\n");
    my_closure4(); printf("\n");
}

// References
//  1. https://stackoverflow.com/questions/18847424/c-create-custom-function-dispatcher-from-variadic-template
//  2. https://stackoverflow.com/questions/42047795/order-of-parameter-pack-expansion
//  3. https://stackoverflow.com/questions/29194858/order-of-function-calls-in-variadic-template-expansion
//  4. https://stackoverflow.com/questions/43553585/how-to-transform-a-variadic-template-argument-to-another-types-for-calling-anoth
//  5. https://stackoverflow.com/questions/12030538/calling-a-function-for-each-variadic-template-argument-and-an-array
//  6. https://stackoverflow.com/questions/687490/how-do-i-expand-a-tuple-into-variadic-template-functions-arguments/12650100#12650100
//  7. https://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer
//  8. https://en.cppreference.com/w/cpp/language/list_initialization#Notes
//  9. https://stackoverflow.com/questions/47207621/build-function-parameters-with-variadic-templates
// 10. https://stackoverflow.com/questions/16868129/how-to-store-variadic-template-arguments
// 11. https://stackoverflow.com/questions/43026550/how-to-define-a-class-that-can-save-variadic-template-arguments
// 12. https://stackoverflow.com/questions/17996003/how-to-save-variable-number-of-arguments-using-variadic-template-arguments
// 13. https://stackoverflow.com/questions/29220248/is-it-possible-to-store-variadic-arguments-into-a-member-variable
// 14. https://stackoverflow.com/questions/4691657/is-it-possible-to-store-a-template-parameter-pack-without-expanding-it
// 15. https://arne-mertz.de/2016/11/modern-c-features-variadic-templates/
// 16. https://gitlab.com/evandro-crr/epos2/tree/master
// 17. https://stackoverflow.com/questions/13108663/storing-functions-call-and-list-of-parameters-to-invoke-later
// 18. https://stackoverflow.com/questions/4926433/saving-function-pointerarguments-for-later-use
// 19. https://appuals.com/fix-cannot-execute-binary-file-exec-format-error-ubuntu/
// 20. https://stackoverflow.com/questions/30926764/extracting-function-argument-types-as-a-parameter-pack
// 21. https://stackoverflow.com/questions/24948277/unpacking-arguments-of-a-functional-parameter-to-a-c-template-class
// 22. https://stackoverflow.com/questions/14783876/c-is-it-possible-to-extract-class-and-argument-types-from-a-member-function
// 23. https://stackoverflow.com/questions/34836104/how-to-extract-a-selected-set-of-arguments-of-a-variadic-function-and-use-them-t
// 24. https://stackoverflow.com/questions/28033251/can-you-extract-types-from-template-parameter-function-signature
// 25. https://stackoverflow.com/questions/11056714/c-type-traits-to-extract-template-parameter-class
// 26. https://stackoverflow.com/questions/32674839/variadic-member-function-of-template-class
// 27. https://stackoverflow.com/questions/49217891/class-member-function-in-variadic-template
// 28. https://stackoverflow.com/questions/50316284/how-to-achieve-variadic-virtual-member-function
// 29. https://stackoverflow.com/questions/15599679/class-member-function-pointer
// 30. https://stackoverflow.com/questions/40855835/how-do-you-typedef-a-function-pointer-type-with-parameter-pack-arguments
// 31. https://stackoverflow.com/questions/8915797/calling-a-function-through-its-address-in-memory-in-c-c
// 32. https://stackoverflow.com/questions/47005664/call-function-by-known-address-c
// 33. https://en.cppreference.com/w/cpp/language/parameter_pack
// 34. https://stackoverflow.com/questions/47037395/forward-types-in-variadic-template-as-values-references-according-to-function-si
// 35. https://eli.thegreenplace.net/2014/perfect-forwarding-and-universal-references-in-c
// 36. https://stackoverflow.com/questions/3836648/structure-or-class-with-variable-number-of-members
// 37. https://stackoverflow.com/questions/43069213/c-class-template-for-automatic-getter-setter-methods-good-bad-practice
// 38. https://stackoverflow.com/questions/13980157/c-class-with-template-member-variable
// 39. https://stackoverflow.com/questions/6261375/how-to-declare-data-members-that-are-objects-of-any-type-in-a-class
// 40. https://stackoverflow.com/questions/1872220/is-it-possible-to-iterate-over-arguments-in-variadic-macros
// 41. https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments
// 42. https://groups.google.com/forum/#!topic/comp.std.c/d-6Mj5Lko_s
// 43. https://stackoverflow.com/questions/27941661/generating-one-class-member-per-variadic-template-argument
// 44. https://stackoverflow.com/questions/14919990/c-how-to-template-a-class-attribute-and-not-the-functions-of-the-class
// 45. https://stackoverflow.com/questions/40204338/template-parameter-pack-attribute
// 46. https://stackoverflow.com/questions/5723619/is-it-possible-to-create-function-local-closures-pre-c11
// 47. https://blog.feabhas.com/2014/03/demystifying-c-lambdas/
// 48. https://stackoverflow.com/questions/1447199/c-closures-and-templates
// 49. http://matt.might.net/articles/c++-template-meta-programming-with-lambda-calculus/
// 50. https://www.gnu.org/software/gcc/gcc-4.4/cxx0x_status.html
// 51. https://stackoverflow.com/questions/25091436/expand-a-parameter-pack-with-a-counter
// 52. https://stackoverflow.com/questions/41623422/c-expand-variadic-template-arguments-into-a-statement
// 53. https://stackoverflow.com/questions/8526598/how-does-stdforward-work
// 54. https://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer/7858971
// 55. https://stackoverflow.com/questions/31204084/is-it-possible-to-call-static-method-form-variadic-template-type-parameter
// 56. https://stackoverflow.com/questions/25680461/variadic-template-pack-expansion
// 57. https://stackoverflow.com/questions/21180346/variadic-template-unpacking-arguments-to-typename
// 58. https://stackoverflow.com/questions/2934904/order-of-evaluation-in-c-function-parameters
// 59. https://stackoverflow.com/questions/9566187/function-parameter-evaluation-order
// 60. https://stackoverflow.com/questions/7728478/c-template-class-function-with-arbitrary-container-type-how-to-define-it
// 61. https://stackoverflow.com/questions/12048221/c11-variadic-template-function-parameter-pack-expansion-execution-order
// 62. https://stackoverflow.com/questions/34957810/variadic-templates-parameter-pack-and-its-discussed-ambiguity-in-a-parameter-li
// 63. https://stackoverflow.com/questions/20588191/error-with-variadiac-template-parameter-pack-must-be-expanded
// 64. https://stackoverflow.com/questions/37200391/multiple-variadic-parameter-pack-for-template-class
// 65. https://stackoverflow.com/questions/34940875/parameter-pack-must-be-at-the-end-of-the-parameter-list-when-and-why
// 

