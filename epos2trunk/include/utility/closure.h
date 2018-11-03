// EPOS Guard Component Declarations

#ifndef __closure_h
#define __closure_h

#include <utility/list.h>

__BEGIN_UTIL

class Closure_Base
{
    /// The Guard class requires access to our the _link private attribute
    friend class Guard;
    typedef List_Elements::Singly_Linked<Closure_Base> Element;

public:
    Closure_Base(): _link(this) {
        db<Synchronizer>(TRC) << "Closure_Base(_link=" << &_link << ") => " << this << endl;
    }

    /// It is not required to have a destructor for this class, then, it is not required for it to
    /// be virtual. But, we add a virtual destructor just for academic purposes of understanding.
    ~Closure_Base() {
        db<Synchronizer>(TRC) << "~Closure_Base(this=" << this << " _link=" << &_link << ")" << endl;
    }

    /// This must to be virtual otherwise the derived classes objects run() method would not be
    /// called when accessed by a base class pointer.
    virtual void run() = 0;

private:
    // Inspired by the thread code
    Element _link; 
};

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
class Closure: public Closure_Base
{
public:
    typedef ReturnType(*Function)(Tn ...);

public:

    Closure(Function _entry, Tn ... an): _entry(_entry)
    {
        db<Synchronizer>(TRC) << "Closure(_entry=" << &_entry << ") => " << this << endl;
        static const unsigned int PARAMETERS_SIZE = SIZEOF<Tn ... >::Result;
        _parameters = new (SYSTEM) char[PARAMETERS_SIZE];

        db<Synchronizer>(TRC) << "Closure(size=" << PARAMETERS_SIZE 
                << " address=" << reinterpret_cast<int *>(_parameters) << ")" << endl;

        pack_helper(_parameters, an ...);
    }

    ~Closure() {
        db<Synchronizer>(TRC) << "Closure(this=" << this << " _entry=" << &_entry 
                << ", _parameters=" << reinterpret_cast<int *>(_parameters) << ")" << endl;

        delete _parameters;
    }

    void run() { 
        char* walker = _parameters;
        ArgumentEvaluteOrderer<ReturnType>{ _entry, unpack_helper<Tn>(walker)... };
    }

    template<typename T>
    static T unpack_helper(char* &_parameters)
    {
        db<Synchronizer>(TRC) << "Closure::unpack_helper(T=" << sizeof( T ) 
                << ", address=" << reinterpret_cast<int *>(_parameters) << ")" << endl;

        char* old = _parameters;
        _parameters += sizeof( T );
        return *reinterpret_cast<T *>( old );
    }

    template<typename Head, typename ... Tail>
    static void pack_helper(char* pointer_address, Head head, Tail ... tail)
    {
        db<Synchronizer>(TRC) << "Closure::pack_helper(T=" << sizeof( Head ) 
                << ", address=" << reinterpret_cast<int *>(pointer_address) << ")" << endl;

        *reinterpret_cast<Head *>(pointer_address) = head;
        pack_helper(pointer_address + sizeof( Head ), tail ...);
    }

    static void pack_helper(char* pointer_address) {}

private:
    Function _entry;
    char* _parameters;
};

__END_UTIL

#endif
