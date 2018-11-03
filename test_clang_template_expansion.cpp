template <int N>
struct Factorial
{
  enum { value = N * Factorial<N-1>::value };
};

template <>
struct Factorial<0>
{
  enum { value = 1 } ;
};

// clang++ -Xclang -ast-print -fsyntax-only test.cpp > test_clang_template_expanded.cpp
// https://stackoverflow.com/questions/4448094/can-we-see-the-template-instantiated-code-by-c-compiler
int main(int argc, char const *argv[])
{
    Factorial<10>();
    return 0;
}
