template <int ...> struct MetaSequenceOfIntegers {
};
template<> struct MetaSequenceOfIntegers<<5, 1, 0>> {
};
template <int AccumulatedSize, typename Tn, int ...GeneratedSequence> struct GeneratorOfIntegerSequence
template<> struct GeneratorOfIntegerSequence<0, int (char, int, char), <>> {
    typedef typename GeneratorOfIntegerSequence<0 + sizeof(char), int (int, char), 0>::type type;
}
template<> struct GeneratorOfIntegerSequence<1, int (int, char), <0>> {
    typedef typename GeneratorOfIntegerSequence<1 + sizeof(int), int (char), 1, 0>::type type;
}
template<> struct GeneratorOfIntegerSequence<5, int (char), <1, 0>> {
    typedef typename GeneratorOfIntegerSequence<5 + sizeof(char), int (), 5, 1, 0>::type type;
}
template<> struct GeneratorOfIntegerSequence<6, int (), <5, 1, 0>> {
    typedef MetaSequenceOfIntegers<5, 1, 0> type;
};
template <int AccumulatedSize, typename Grouper, typename Head, typename ...Tail, int ...GeneratedSequence> struct GeneratorOfIntegerSequence<AccumulatedSize, type-parameter-0-1 (type-parameter-0-2, type-parameter-0-3...), <GeneratedSequence...>> {
    typedef typename GeneratorOfIntegerSequence<AccumulatedSize + sizeof(Head), Grouper (Tail...), AccumulatedSize, GeneratedSequence...>::type type;
};
template <int AccumulatedSize, typename Grouper, int ...GeneratedSequence> struct GeneratorOfIntegerSequence<AccumulatedSize, type-parameter-0-1 (), <GeneratedSequence...>> {
    typedef MetaSequenceOfIntegers<GeneratedSequence...> type;
};
template <int ...Sequence> void intergers_sequencer_generator(MetaSequenceOfIntegers<Sequence...>) {
    int array[] = {Sequence...};
}
template<> void intergers_sequencer_generator<<5, 1, 0>>(MetaSequenceOfIntegers<5, 1, 0>) {
    int array[] = {5, 1, 0};
}
int main(int argc, const char *argv[]) {
    intergers_sequencer_generator(GeneratorOfIntegerSequence<0, int (char, int, char)>::type());
    return 0;
}
