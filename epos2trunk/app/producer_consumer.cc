// EPOS Synchronizer Component Test Program

#include <utility/ostream.h>
#include <thread.h>
#include <alarm.h>
#include <utility/guard.h>
#include <utility/future.h>
#include <condition.h>

using namespace EPOS;

const int iterations = 100;

OStream cout;

Guard buffer_guard;
Condition full;
Condition empty;
const int BUF_SIZE = 16;
char buffer[BUF_SIZE];
int buffer_count = 0;
int out = 0;
int in = 0;


void load(Future<char> * item)
{
    int old_out = out;
    item->resolve(buffer[out]);
    cout << "C<-" << buffer[out] << "\t";
    out = (out + 1) % BUF_SIZE;
    buffer_count--;
    if((in == old_out) && (buffer_count+1)) // checking whether the buffer WAS full
        full.signal(); // wakeup waiting producer
}

void store(char item)
{
    buffer[in] = item;
    cout << "P->" << item << "\t";
    in = (in + 1) % BUF_SIZE;
    buffer_count++;
    if (buffer_count-1) // checking whether the buffer WAS empty
        empty.signal(); // wakeup waiting consumer
}

int consumer()
{
    for(int i = 0; i < iterations; i++) {
        Future<char> * item = new Future<char>();
        if(!buffer_count) // checking whether the buffer IS empty
            empty.wait(); // wait somehow
        buffer_guard.submit(&load, item); // load item from buffer
        item->get_value();  // consume - part1
        delete(item);       // consume - part2
        Alarm::delay(5000); // consume - part3
    }
    return 0;
}

int producer()
{
    for(int i = 0; i < iterations; i++) {
        Alarm::delay(5000);    // produce - part1
        char item = 'a' + in;  // produce - part2
        if((in == out) && (buffer_count)) // checking whether the buffer IS full
            full.wait();// wait somehow
        buffer_guard.submit(&store, item); // store item on buffer
    }
    return 0;
}

int main()
{
    cout << "Main started!" << endl;

    //Thread * cons = new Thread(&consumer);
    Thread * prod = new Thread(&producer);

    //cons->join();
    prod->join();

    cout << "The end!" << endl;

    //delete cons;
    delete prod;

    return 0;
}