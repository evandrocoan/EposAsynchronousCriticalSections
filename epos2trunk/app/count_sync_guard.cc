// EPOS Synchronizer Component Test Program

#include <guard.h>
#include <thread.h>
#include <machine.h>
#include <utility/guard.h>
#include <alarm.h>

using namespace EPOS;

static volatile int counter = 0;
static const int iterations = 1e5;

Guard counter_guard;
Guard display_guard;
Thread * pool[5];

void a_begin() {
    db<Synchronizer>(WRN)   << "A: begin (counter = " << counter 
                            << ")" << endl;
}

void b_begin() {
    db<Synchronizer>(WRN)   << "B: begin (counter = " << counter 
                            << ")" << endl;
}

void c_begin() {
    db<Synchronizer>(WRN)   << "C: begin (counter = " << counter 
                            << ")" << endl;
}

void d_begin() {
    db<Synchronizer>(WRN)   << "D: begin (counter = " << counter 
                            << ")" << endl;
}

void e_begin() {
    db<Synchronizer>(WRN)   << "E: begin (counter = " << counter 
                            << ")" << endl;
}

void increment_counter(){
    counter = counter + 1;
    // db<Synchronizer>(WRN)   << "increment_counter (counter = " << counter 
    //                         << ")" << endl;
}

int mythread(int arg) {
    switch(arg) {
        case 1:
            display_guard.submit(new Critical_Section(&a_begin));
            break;
        case 2:
            display_guard.submit(new Critical_Section(&b_begin));
            break;
        case 3:
            display_guard.submit(new Critical_Section(&c_begin));
            break;        
        case 4:
            display_guard.submit(new Critical_Section(&d_begin));
            break;
        case 5:
            display_guard.submit(new Critical_Section(&e_begin));
            break;
    }

    for (int i = iterations; i > 0 ; i--) {
        counter_guard.submit(new Critical_Section(&increment_counter));
    }

    return 0;
}

int main()
{
    db<Synchronizer>(WRN)   << "main: begin (counter = " << counter 
                            << ")" << endl;

    pool[0] = new Thread(&mythread, 1);
    pool[1] = new Thread(&mythread, 2);
    pool[2] = new Thread(&mythread, 3);
    pool[3] = new Thread(&mythread, 4);
    pool[4] = new Thread(&mythread, 5);

    db<Synchronizer>(WRN)   << "main: start joining the threads (counter = " << counter 
                            << ")" << endl;

    // join waits for the threads to finish
    for(int i = 0; i < 5; i++) {
        pool[i]->join();
    }

    db<Synchronizer>(WRN)   << "main: done with both (counter = " << counter 
                            << ")" << endl;

    for(int i = 0; i < 5; i++)
        delete pool[i];

    db<Synchronizer>(WRN)   << "main: exiting (counter = " << counter 
                            << ")" << endl;

    return 0;
}
