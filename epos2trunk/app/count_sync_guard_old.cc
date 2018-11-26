// EPOS Synchronizer Component Test Program

#include <utility/ostream.h>
#include <thread.h>
#include <machine.h>
#include <utility/guard.h>
#include <alarm.h>

using namespace EPOS;

OStream cout;

static volatile int counter = 0;
static const int iterations = 1e8;

Guard counter_guard;
Guard display_guard;
Thread * pool[5];

void a_begin(){ LOG( "A: begin (counter = " << counter << ")" << endl); }
void b_begin(){ LOG( "B: begin (counter = " << counter << ")" << endl); }
void c_begin(){ LOG( "C: begin (counter = " << counter << ")" << endl); }
void d_begin(){ LOG( "D: begin (counter = " << counter << ")" << endl); }
void e_begin(){ LOG( "E: begin (counter = " << counter << ")" << endl); }

void increment_counter(){ 
    counter = counter + 1;     
}

void a_end(){ LOG( "A: end (counter = " << counter << ")" << endl); }
void b_end(){ LOG( "B: end (counter = " << counter << ")" << endl); }
void c_end(){ LOG( "C: end (counter = " << counter << ")" << endl); }
void d_end(){ LOG( "D: end (counter = " << counter << ")" << endl); }
void e_end(){ LOG( "E: end (counter = " << counter << ")" << endl); }

int mythread(int arg) {
    switch(arg) {
        case 1:
            display_guard.submit(&a_begin);
            break;
        case 2:
            display_guard.submit(&b_begin);
            break;
        case 3:
            display_guard.submit(&c_begin);
            break;        
        case 4:
            display_guard.submit(&d_begin);
            break;
        case 5:
            display_guard.submit(&e_begin);
            break;
    }

    for (int i = iterations; i > 0 ; i--) {
        counter_guard.submit_no_sequencer(&increment_counter, arg);
    }

    switch(arg) {
    case 1:
        display_guard.submit(&a_end);
        break;
    case 2:
        display_guard.submit(&b_end);
        break;
    case 3:
        display_guard.submit(&c_end);
        break;        
    case 4:
        display_guard.submit(&d_end);
        break;
    case 5:
        display_guard.submit(&e_end);
        break;
}
    
    return 0;
}

int main()
{
    LOG( "main: begin (counter = " << counter << ")" << endl);

    LOG( "the size of a base critical section is " << sizeof(Critical_Section_Base) << " bytes" << endl); // 8 bytes
    LOG( "the size of a void critical section is " << sizeof(Void_Critical_Section) << " bytes" << endl); // 12 bytes

    pool[0] = new Thread(&mythread, 1);
    pool[1] = new Thread(&mythread, 2);
    //pool[2] = new Thread(&mythread, 3);
    //pool[3] = new Thread(&mythread, 4);
    //pool[4] = new Thread(&mythread, 5);

    // join waits for the threads to finish
    for(int i = 0; i < 2; i++) {
        pool[i]->join();
    }

    LOG( "main: done with all threads (counter = " << counter << ")"<< endl);

//    counter_guard.show();

    for(int i = 0; i < 2; i++)
        delete pool[i];

    return 0;
}
