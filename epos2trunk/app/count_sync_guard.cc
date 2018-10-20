// EPOS Synchronizer Component Test Program

#include <utility/ostream.h>
#include <thread.h>
#include <machine.h>
#include <guard.h>
#include <alarm.h>

using namespace EPOS;

OStream cout;

static volatile int counter = 0;
static const int iterations = 10;

Guard counter_guard;
Guard display_guard;
Thread * pool[5];


void a_begin(){
    cout << "A: begin (counter = " << counter << ")" << endl;
}

void b_begin(){
    cout << "B: begin (counter = " << counter << ")" << endl;
}

void c_begin(){
    cout << "C: begin (counter = " << counter << ")" << endl;
}

void d_begin(){
    cout << "D: begin (counter = " << counter << ")" << endl;
}

void e_begin(){
    cout << "E: begin (counter = " << counter << ")" << endl;
}

void increment_counter(){
    counter = counter + 1;
}

int mythread(int arg) {
    switch(arg) {
        case 1:
            display_guard(new Critical_Section(&a_begin()));
            break;
        case 2:
            display_guard(new Critical_Section(&b_begin()));
            break;
        case 3:
            display_guard(new Critical_Section(&c_begin()));
            break;        
        case 4:
            display_guard(new Critical_Section(&d_begin()));
            break;
        case 5:
            display_guard(new Critical_Section(&e_begin()));
            break;
    }

    display_guard.submit()
    
    for (int i = iterations; i > 0 ; i--) {
        counter_guard.submit(new Critical_Section(&increment_counter));
    }
    
    return 0;
}

int main()
{
    cout << "main: begin (counter = " << counter << ")" << endl;

    pool[0] = new p1(&mythread, 1);
    pool[1] = new p2(&mythread, 2);
    pool[2] = new p2(&mythread, 3);
    pool[3] = new p2(&mythread, 4);
    pool[4] = new p2(&mythread, 5);

    // join waits for the threads to finish
    for(int i = 0; i < 5; i++) {
        phil[i]->join();
    }

    cout << "main: done with both (counter = " << counter << ")"<< endl;

    for(int i = 0; i < 5; i++)
        delete pool[i];

    return 0;
}
