template<char c> struct add_three {
    enum { value = c+3 };
};

template <char... Chars> struct EncryptCharsA {
    static const char value[sizeof...(Chars) + 1];
};

template<char... Chars>
char const EncryptCharsA<Chars...>::value[sizeof...(Chars) + 1] = {
    add_three<Chars>::value...
};

// https://stackoverflow.com/questions/3492742/compile-time-string-manipulation-with-variadic-templates
// /usr/bin/clang++ -Xclang -ast-print -fsyntax-only test_debugger.cpp -DTINYFORMAT_FORMATTER_DEBUGGER_LEVEL=127 > main.exe
int main() {
    EncryptCharsA<'A','B','C'>::value;
    // prints "DEF"
}
