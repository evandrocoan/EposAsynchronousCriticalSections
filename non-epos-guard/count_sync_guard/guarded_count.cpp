#include <iostream>
#include "pthread.h"

#define ASM                     __asm__ __volatile__

using namespace std;

class Critical_Section
{
public:
    friend class Guard;
    typedef void (Function)();

public:
    Critical_Section(Function * h): _handler(h), _next(0) {}
    ~Critical_Section() {}

    void run() { _handler(); }

public:
    Function * _handler;
    Critical_Section * _next;
};

// BEGIN # ATOMIC OPERATIONS
static Critical_Section* fas(Critical_Section * volatile & value, Critical_Section * replacement) {
    ASM("xchg %2, %1" : "=r"(replacement) : "r"(replacement), "m"(value) : "memory");
    return replacement;
}
static Critical_Section* cas(Critical_Section * volatile & value, Critical_Section * compare, Critical_Section * replacement, bool vouch) {
    if (value != 0 && vouch)
        cout << "[value= " << value << ", compare = " << compare << ", replacement= " << replacement << "]" << endl;
    ASM("lock cmpxchgl %2, %3\n" : "=a"(compare) : "a"(compare), "r"(replacement), "m"(value) : "memory");
    return compare;
}
// END # ATOMIC OPERATIONS

class Guard
{
public:
    typedef Critical_Section::Function Function;

private:
    static const unsigned int null = 0;
    static const unsigned int done = 1;

public:
    Guard(): _head(0), _tail(0){}
    ~Guard() {}

    void submit(Function * f){
        Critical_Section * volatile cur = vouch(new Critical_Section(f));
        if (cur != reinterpret_cast<Critical_Section *>(null)) do {
            cur->run();
            cur = clear();
        } while ( cur != reinterpret_cast<Critical_Section *> (null));
    }

    Critical_Section * volatile vouch(Critical_Section * item){
        Critical_Section * volatile last = fas(_tail, item);
        if (last){
            if (cas(last->_next, reinterpret_cast<Critical_Section *>(null), item, true) == reinterpret_cast<Critical_Section *>(null))
                return reinterpret_cast<Critical_Section *>(null);
            delete last;
        }
        _head = item;
        return item;
    }

    Critical_Section * clear(){
        Critical_Section * volatile item = _head;
        Critical_Section * volatile next = fas(item->_next, reinterpret_cast<Critical_Section *>(done));
        bool mine = true;
        if (!next)
            mine = cas(_tail, item, reinterpret_cast<Critical_Section *> (null), false) == item ;
        cas(_head, item, next, false);
        if (mine)
            delete item;
        return next;
    }
public:
    Critical_Section * volatile _head;
    Critical_Section * volatile _tail;
};

pthread_t pool[4];
static pthread_mutex_t mutex;
int counter = 0;
static const int iterations = 1e7;
Guard counter_guard;
Guard display_guard;

void increment_counter(){ counter = counter + 1; }

void a_begin() { cout << "[A] : begin " << counter << endl; }
void b_begin() { cout << "[B] : begin " << counter << endl; }
void c_begin() { cout << "[C] : begin " << counter << endl; }
void d_begin() { cout << "[D] : begin " << counter << endl; }

void a_end()   { cout << "[A] : end " << counter << endl; }
void b_end()   { cout << "[B] : end " << counter << endl; }
void d_end()   { cout << "[D] : end " << counter << endl; }
void c_end()   { cout << "[C] : end " << counter << endl; }

void * mythread(void * id) {

    long id_long = (long)(id);
    char arg = (char)(id_long+64);

    switch(id_long){
        case 1:
            display_guard.submit(a_begin);
            break;
        case 2:
            display_guard.submit(b_begin);
            break;
        case 3:
            display_guard.submit(c_begin);
            break;
        case 4:
            display_guard.submit(d_begin);
            break;
    }

    for (int i = iterations; i > 0 ; i--) {
        counter_guard.submit(increment_counter);
    }

    switch(id_long){
        case 1:
            display_guard.submit(a_end);
            break;
        case 2:
            display_guard.submit(b_end);
            break;
        case 3:
            display_guard.submit(c_end);
            break;
        case 4:
            display_guard.submit(d_end);
            break;
    }

    return 0;
}

int main(){

    cout << "Main begin!" << endl;

    void * one = (void *) 1;
    void * two = (void *) 2;
    void * three = (void *) 3;
    void * four = (void *) 4;

    pthread_create(&pool[0], NULL, mythread, one);
    pthread_create(&pool[1], NULL, mythread, two);
    pthread_create(&pool[2], NULL, mythread, three);
    pthread_create(&pool[3], NULL, mythread, four);

    void* status;

    pthread_join(pool[0], &status);
    pthread_join(pool[1], &status);
    pthread_join(pool[2], &status);
    pthread_join(pool[3], &status);

    cout << "[counter]= " << counter << endl;

    pthread_mutex_lock(&mutex);
    cout << "The end!" << endl;
    pthread_mutex_unlock(&mutex);

    return 0;
}

//  g++ count-cpp/guarded_count.cpp -o count-out/guarded_count.out -lpthread -m32
//  sudo chrt --rr 99 ./guarded_count &> test_guarded.log

