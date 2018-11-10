template <int N> struct Factorial {
    enum  : int {
        value = N * Factorial<N - 1>::value
    };
};
template<> struct Factorial<10> {
    enum  : int {
        value = 10 * Factorial<10 - 1>::value
    };
};
template<> struct Factorial<9> {
    enum  : int {
        value = 9 * Factorial<9 - 1>::value
    };
};
template<> struct Factorial<8> {
    enum  : int {
        value = 8 * Factorial<8 - 1>::value
    };
};
template<> struct Factorial<7> {
    enum  : int {
        value = 7 * Factorial<7 - 1>::value
    };
};
template<> struct Factorial<6> {
    enum  : int {
        value = 6 * Factorial<6 - 1>::value
    };
};
template<> struct Factorial<5> {
    enum  : int {
        value = 5 * Factorial<5 - 1>::value
    };
};
template<> struct Factorial<4> {
    enum  : int {
        value = 4 * Factorial<4 - 1>::value
    };
};
template<> struct Factorial<3> {
    enum  : int {
        value = 3 * Factorial<3 - 1>::value
    };
};
template<> struct Factorial<2> {
    enum  : int {
        value = 2 * Factorial<2 - 1>::value
    };
};
template<> struct Factorial<1> {
    enum  : int {
        value = 1 * Factorial<1 - 1>::value
    };
};
template<> struct Factorial<0> {
    enum  : int {
        value = 1
    };
};
int main(int argc, const char *argv[]) {
    Factorial<10>();
    return 0;
}
