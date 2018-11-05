template<int ...>
struct MetaSequenceOfIntegers { };

template<int AccumulatedSize, typename Tn, int... GeneratedSequence>
struct GeneratorOfIntegerSequence;

template<int AccumulatedSize, typename Grouper, typename Head, typename... Tail, int... GeneratedSequence>
struct GeneratorOfIntegerSequence< AccumulatedSize, Grouper( Head, Tail... ), GeneratedSequence... >
{
    typedef typename GeneratorOfIntegerSequence
            < AccumulatedSize + sizeof(Head), Grouper( Tail... ), GeneratedSequence...,  AccumulatedSize
            >::type type;
};

template<int AccumulatedSize, typename Grouper, int... GeneratedSequence>
struct GeneratorOfIntegerSequence<AccumulatedSize, Grouper(), GeneratedSequence...>
{
  typedef MetaSequenceOfIntegers<GeneratedSequence...> type;
};


template<int ...Sequence>
void intergers_sequencer_generator(MetaSequenceOfIntegers<Sequence...>) {
    int array[] = {Sequence...};
}

// clang++ -Xclang -ast-print -fsyntax-only parameter_pack_sequencer_size_generator.cpp > parameter_pack_sequencer_size_expanded.cpp
// https://stackoverflow.com/questions/4448094/can-we-see-the-template-instantiated-code-by-c-compiler
int main(int argc, char const *argv[])
{
    intergers_sequencer_generator( GeneratorOfIntegerSequence< 0, int(char, int, char) >::type() );
    return 0;
}
