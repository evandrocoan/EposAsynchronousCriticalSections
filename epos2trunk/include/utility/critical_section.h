// EPOS Guard Component Declarations

#ifndef __critical_section_h
#define __critical_section_h

#include <utility/list.h>

__BEGIN_UTIL

class Critical_Section_Base
{
    /// The Guard class requires access to our the _link private attribute
    friend class Guard;
    typedef List_Elements::Singly_Linked<Critical_Section_Base> Element;

public:
    Critical_Section_Base();

    /// It is not required to have a destructor for this class, then, it is not required for it to
    /// be virtual. But, we add a virtual destructor just for academic purposes of understanding.
    virtual ~Critical_Section_Base();


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
class Critical_Section: public Critical_Section_Base
{
public:
    typedef ReturnType(*Function)(Tn ...);

public:

    Critical_Section(Function _entry, Tn ... an): _entry(_entry)
    {
        db<Synchronizer>(TRC) << "Critical_Section(_entry=" << _entry << ") => " << this << endl;
        static const unsigned int PARAMETERS_SIZE = SIZEOF<Tn ... >::Result;
        _parameters = new (SYSTEM) char[PARAMETERS_SIZE];

        db<Synchronizer>(TRC) << "Critical_Section(size=" << PARAMETERS_SIZE << " address=" << _parameters << ")" << endl;
        pack_helper(_parameters, an ...);
    }

    ~Critical_Section()
    {
        db<Synchronizer>(TRC) << "Critical_Section(this=" << this << " _entry=" << _entry << ", _parameters=" << _parameters << ")" << endl;
        delete _parameters;
    }

    void run() 
    { 
        char* walker = _parameters;
        ArgumentEvaluteOrderer<ReturnType>{ _entry, unpack_helper<Tn>(walker)... };
    }

    template<typename T>
    static T unpack_helper(char* &_parameters)
    {
        db<Synchronizer>(TRC) << "Critical_Section::unpack_helper(T=" << sizeof( T ) << ", address=" << _parameters << ")" << endl;
        char* old = _parameters;

        _parameters += sizeof( T );
        return *reinterpret_cast<T *>( old );
    }

    template<typename Head, typename ... Tail>
    static void pack_helper(char* pointer_address, Head head, Tail ... tail)
    {
        db<Synchronizer>(TRC) << "Critical_Section::unpack_helper(T=" << sizeof( Head ) << ", address=" << pointer_address << ")" << endl;

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
