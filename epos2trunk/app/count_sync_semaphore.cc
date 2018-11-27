// EPOS Synchronizer Component Test Program
#define DEBUG_SYNC

#include <utility/ostream.h>
#include <utility/debug_sync.h>
#include <semaphore.h>
#include <thread.h>
#include <machine.h>

using namespace EPOS;
int counter = 0;

Semaphore counter_lock;

int mythread(char arg) {
    LOG( arg << ": begin " << counter << endl )

    for (int i = 0; i < 1e6; i++) {
        counter_lock.p();
        counter = counter + 1;
        if ((counter % 100000) == 0)
            LOG("increment_counter (counter=" << counter << ")" << endl )
        counter_lock.v();
    }

    LOG( arg << ": done " << counter << endl )
    return 0;
}

int main()
{
    LOG( endl )
    LOG( "main: begin (counter = " << counter << ")" << endl )

    Thread * p1 = new Thread(&mythread, 'A');
    Thread * p2 = new Thread(&mythread, 'B');
    // Thread * p3 = new Thread(&mythread, 'C');
    // Thread * p4 = new Thread(&mythread, 'D');
    // Thread * p5 = new Thread(&mythread, 'E');

    LOG( "main: joining all threads (counter=" << counter << ")" << endl )
    p1->join();
    p2->join();
    // p3->join();
    // p4->join();
    // p5->join();
    LOG( "main: done with both all threads(counter = " << counter << ")" << endl )

    delete p1;
    delete p2;
    // delete p3;
    // delete p4;
    // delete p5;

    return 0;
}
