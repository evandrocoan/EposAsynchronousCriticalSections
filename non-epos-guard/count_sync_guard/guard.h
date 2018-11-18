#include <mutex>
#include <atomic>
#include <cassert>

#include <iomanip>
#include <iostream>
#include <condition_variable>

#define ASM __asm__ __volatile__


// You can define it anywhere before including this file
// #define DEBUG_SYNC

#ifdef DEBUG_SYNC
    std::recursive_mutex _debug_syncronized_semaphore_lock;

    #define DB(...) do { \
        _debug_syncronized_semaphore_lock.lock(); \
            std::cout << __VA_ARGS__; \
        _debug_syncronized_semaphore_lock.unlock(); } while(0);

    #define LOG(...) DB(__VA_ARGS__)

#else
    #define DB(...)
    #define LOG(...) std::cout << __VA_ARGS__;

#endif


template<typename T>
static T fas(T volatile & value, T replacement)
{
    ASM("xchg %2, %1"
            : "=r"(replacement)
            : "r"(replacement), "m"(value)
            : "memory");
    return replacement;
}

template<typename T>
static T cas(T volatile & value, T compare, T replacement)
{
    ASM("lock cmpxchgl %2, %3\n"
            : "=a"(compare)
            : "a"(compare), "r"(replacement), "m"(value) : "memory");
    return compare;
}


class Display
{
private:
    static const int LINES = 25;
    static const int COLUMNS = 8000;
    static const int TAB_SIZE = 8;

    // Special characters
    enum {
        ESC  = 0x1b,
        CR   = 0x0d,
        LF   = 0x0a,
        TAB  = 0x09,
    };

public:
    Display() {}

    static void clear() {
        _line = 0;
        _column = 0;
        escape();
        put('2');
        put('J');
    };

    static void putc(char c) {
        switch(c) {
        case '\n':
            scroll();
            _line++;
            break;
        case '\t':
            put(TAB);
            _column += TAB_SIZE;
            break;
        default:
            _column++;
            put(c);
            if(_column >= COLUMNS) scroll();
        }
    };

    static void puts(const char * s) {
        while(*s != '\0')
            putc(*s++);
    }

    static void geometry(int * lines, int * columns) {
        *lines = LINES;
        *columns = COLUMNS;
    }

    static void position(int * line, int * column) {
        *line = _line;
        *column = _column;
    }
    static void position(int line, int column) {
        _line = line;
        _column = column;
        escape();
        puti(_line);
        put(';');
        puti(_column);
        put('H');
    }

private:
    static void put(char c) {
        std::cout << c;
    }

    static void escape() {
        put(ESC);
        put('[');
    }

    static void puti(unsigned char value) {
        unsigned char digit = '0';
        while(value >= 100) {
            digit++;
            value -= 100;
        }
        put(digit);

        digit = '0';
        while(value >= 10) {
            digit++;
            value -= 10;
        }
        put(digit);

        put('0' + value);
    }

    static void scroll() {
        put(CR);
        put(LF);
        _column = 0;
    }

    static void init() {
        _line = 0;
        _column = 0;
    }

private:
    static int _line;
    static int _column;
};

int Display::_line;
int Display::_column;


template<typename FutureType>
class Future
{
public:
    Future(): _is_resolved(false), _size(0) {
        DB( "Future(_is_resolved=" << _is_resolved
                << ", _condition=" << _size
                << ") => " << this << std::endl )
    }

    ~Future() {
        DB( "~Future(this=" << this << ")" << std::endl );
    }

    FutureType get_value() {
        DB( "Future::get_value(this=" << this
                << " _is_resolved=" << _is_resolved
                << " _condition=" << _size
                <<  ")" << std::endl )

        if(!_is_resolved) {
            ++_size;

            std::unique_lock<std::mutex> lock(_mutex);
            _condition.wait(lock);
        }
        return _value;
    }

    void resolve(FutureType value) {
        DB( "Future::resolve(this=" << this
                << " _is_resolved=" << _is_resolved
                << " _condition=" << _size
                <<  ")" << std::endl )
        assert(!_is_resolved);
        // If `resolve()` was called and the instruction pointer got until here,
        // and the thread is unscheduled, and another thread call `resolve()`,
        // then, the `assert` will not work, if the resolve() call is not atomic.
        _value = value;
        _is_resolved = true;
        _condition.notify_all();
    }

private:
    bool _is_resolved;
    FutureType _value;
    std::atomic<int> _size;

    std::mutex _mutex;
    std::condition_variable _condition;
};


// SIZEOF Type Package
template<typename ... Tn>
struct SIZEOF
{ static const unsigned int Result = 0; };

template<typename T1, typename ... Tn>
struct SIZEOF<T1, Tn ...>
{ static const unsigned int Result = sizeof(T1) + SIZEOF<Tn ...>::Result ; };

// https://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer/7858971
template<int ...>
struct MetaSequenceOfIntegers { };

template<int AccumulatedSize, typename Tn, int... GeneratedSequence>
struct GeneratorOfIntegerSequence;

template<int AccumulatedSize, typename Grouper, typename Head, typename... Tail, int... GeneratedSequence>
struct GeneratorOfIntegerSequence< AccumulatedSize, Grouper( Head, Tail... ), GeneratedSequence... >
{
    typedef typename GeneratorOfIntegerSequence
            < AccumulatedSize + sizeof(Head), Grouper( Tail... ), GeneratedSequence..., AccumulatedSize
            >::type type;
};

template<int AccumulatedSize, typename Grouper, int... GeneratedSequence>
struct GeneratorOfIntegerSequence<AccumulatedSize, Grouper(), GeneratedSequence...>
{
  typedef MetaSequenceOfIntegers<GeneratedSequence...> type;
};


// https://stackoverflow.com/questions/34957810/variadic-templates-parameter-pack-and-its-discussed-ambiguity-in-a-parameter-li
template<typename Tn>
class Closure;

template<typename ReturnType, typename... Tn>
class Closure<ReturnType( Tn... )>
{
public:
    typedef ReturnType(*Function)(Tn ...);
    static const unsigned int PARAMETERS_COUNT = sizeof...( Tn );
    static const unsigned int PARAMETERS_LENGTH = SIZEOF<Tn ...>::Result;

private:
    Function _entry;
    char* _parameters;

public:
    Closure(Function _entry, Tn ... an): _entry(_entry)
    {
        DB( "Closure(_entry=" << reinterpret_cast<void *>(_entry)
                << ", PARAMETERS_COUNT=" << PARAMETERS_COUNT
                << ", PARAMETERS_LENGTH=" << PARAMETERS_LENGTH
                << ", sizeof=" << sizeof(*this) << ") => " << this << std::endl )

        if(PARAMETERS_LENGTH)
            _parameters = new char[PARAMETERS_LENGTH];

        pack_helper( _parameters, an ... );
    }

    ~Closure() {
        DB( "~Closure(this=" << this
                << ", _entry=" << reinterpret_cast<void *>(_entry)
                << ", PARAMETERS_COUNT=" << PARAMETERS_COUNT
                << ", PARAMETERS_LENGTH=" << PARAMETERS_LENGTH
                << ", sizeof=" << sizeof(*this) << ")" << std::endl )

        if(PARAMETERS_LENGTH)
            delete _parameters;
    }

    inline ReturnType run() {
        return operator()();
    }

    inline ReturnType operator()() {
        return _unpack_and_run( typename GeneratorOfIntegerSequence< 0, int(Tn...) >::type() );
    }

private:
    template<int ...Sequence>
    inline ReturnType _unpack_and_run(MetaSequenceOfIntegers<Sequence...>)
    {
        DB( "Closure::_unpack_and_run(this=" << this << ")" << std::endl )
        return _entry( unpack_helper<Sequence, Tn>()... );
    }

    template<const int position, typename T>
    inline T unpack_helper()
    {
        DB( "Closure::unpack_helper(Head=" << sizeof( T )
                << ", address=" << reinterpret_cast<int *>(_parameters + position)
                << "(" << reinterpret_cast<int *>(_parameters) << ")"
                << ", position=" << position << ")" << std::endl )

        return *reinterpret_cast<T *>( _parameters + position );
    }

public:
    template<typename Head, typename ... Tail>
    static void pack_helper(char* pointer_address, Head head, Tail ... tail)
    {
        DB( "Closure::pack_helper(Head=" << sizeof( Head )
                << ", address=" << reinterpret_cast<int *>(pointer_address) << ")" << std::endl )

        *reinterpret_cast<Head *>(pointer_address) = head;
        pack_helper(pointer_address + sizeof( Head ), tail ...);
    }

    static void pack_helper(char* pointer_address) {}
};


class Critical_Section_Base
{
    friend class Guard;
    Critical_Section_Base* _next;

public:
    Critical_Section_Base(): _next(this) {
        DB( "Critical_Section_Base(_next=" << _next
                << ") => " << this << std::endl )
    }

    virtual ~Critical_Section_Base() {
        DB( "~Critical_Section_Base(this=" << this
                << " _next=" << _next << ")" << std::endl )
    }

    virtual void start() = 0;
};

template<typename... Tn>
class Critical_Section: public Critical_Section_Base, private Closure<void( Tn... )>
{
    using Closure<void( Tn... )>::run;

public:
    Critical_Section(void(*_entry)(Tn ...), Tn ... an) :
            Closure<void( Tn... )>::Closure( _entry, an ... )
    {
        DB( "Critical_Section(_entry="
                << reinterpret_cast<void *>(_entry) << ") => " << this << std::endl )
    }

    inline void start() {
        run();
    }
};


class Guard
{
    std::atomic<int> _size;
    void* _sequencer;

    Critical_Section_Base* _head;
    Critical_Section_Base* _tail;

    // static const unsigned int NULL = 0;
    static const unsigned int DONE = 1;

public:
    Guard() : _size(0), _sequencer(0), _head(0), _tail(0)
    {
        DB( "Guard(head=" << _head << ", tail=" << _tail
                << ") => " << this << std::endl )
    }

    ~Guard() {
        DB( "~Guard(this=" << this << ")" << std::endl )
    }

    template<typename ... Tn>
    void submit( void(*entry)( Tn ... ), Tn ... an )
    {
        // Creates a closure with the critical section parameters
        Critical_Section<Tn ...>* cs = new Critical_Section<Tn ...>(entry, an ...);

        Critical_Section_Base * cur = vouch(cs);
        if (cur != reinterpret_cast<Critical_Section_Base *>(NULL)) do {
            _sequencer = get_thread_id();
            cur->start();
            cur = clear();
        } while (cur != reinterpret_cast<Critical_Section_Base *>(NULL));
    }

    Critical_Section_Base* vouch(Critical_Section_Base * item)
    {
        DB( "Guard::vouch(this=" << this
                << " head=" << _head
                << " tail=" << _tail
                << " thread=" << get_thread_id()
                << " sequencer=" << _sequencer
                << " size=" << _size + 1
                << " item=" << item )
        ++_size;

        item->_next = reinterpret_cast<Critical_Section_Base *>(NULL);
        Critical_Section_Base * last = fas( _tail, item );
        DB( " last=" << last << ")" << std::endl )

        if( last ) {
            if( cas( last->_next, reinterpret_cast<Critical_Section_Base *>(NULL), item )
                        == reinterpret_cast<Critical_Section_Base *>(NULL) )

                return reinterpret_cast<Critical_Section_Base *>(NULL);
            delete last;
        }
        _head = item;
        return item;
    }

    Critical_Section_Base * clear()
    {
        DB( "Guard::clear(this=" << this
                << " head=" << _head
                << " tail=" << _tail
                << " thread=" << get_thread_id()
                << " sequencer=" << _sequencer
                << " size=" << _size - 1 )
        --_size;

        Critical_Section_Base * item = _head;
        Critical_Section_Base * next = fas( item->_next, reinterpret_cast<Critical_Section_Base *>(DONE) );
        DB( " next=" << next << ")" << std::endl )

        bool mine = true;
        if( !next ) {
            mine = cas( _tail, item, reinterpret_cast<Critical_Section_Base *> (NULL) ) == item;
        }

        cas( _head, item, next) ;
        if( mine ) {
            delete item;
        }
        return next;
    }

    static void* get_thread_id() {
        std::stringstream ss;
        ss << std::this_thread::get_id();
        return reinterpret_cast<void *>( std::stoull( ss.str() ) );
    }
};

