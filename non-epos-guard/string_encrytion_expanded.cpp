template <char c> struct add_three {
    enum  {
        value = c + 3
    };
};
template<> struct add_three<'A'> {
    enum  {
        value = 'A' + 3
    };
};
template<> struct add_three<'B'> {
    enum  {
        value = 'B' + 3
    };
};
template<> struct add_three<'C'> {
    enum  {
        value = 'C' + 3
    };
};
template <char ...Chars> struct EncryptCharsA {
    static const char value[sizeof...(Chars) + 1];
};
template<> struct EncryptCharsA<<'A', 'B', 'C'>> {
    static const char value[4];
};
const char value[sizeof...(Chars) + 1] = {add_three<Chars>::value...};
int main() {
    EncryptCharsA<'A', 'B', 'C'>::value;
}
const char value[4] = {add_three<'A'>::value, add_three<'B'>::value, add_three<'C'>::value};
