// EPOS Synchronizer Component Test Program

#include <utility/ostream.h>
#include <thread.h>
#include <alarm.h>
#include <utility/guard.h>
#include <utility/future.h>
#include <condition.h>
#include <clock.h>
#include <utility/random.h>

using namespace EPOS;

const int iterations = 100;

OStream cout;
Clock clock;

Guard buffer_guard;
Condition full;
Condition empty;
const int BUF_SIZE = 16;
char buffer[BUF_SIZE];
int buffer_count = 0;
int total_count = 0;
int out = 0;
int in = 0;

void check_load(Future<bool> * wait){
    if(buffer_count > 0){ // checking whether the buffer IS empty
        wait->resolve(false);
    } else {
        wait->resolve(true);
    }
}

void load(Future<char> * item)
{
    item->resolve(buffer[out]);
    cout << "C<-" << buffer[out] << "\t";
    total_count++;
    if (total_count > 10){
        cout << "\n";
        total_count = 0;
    }
    out = (out + 1) % BUF_SIZE;
    buffer_count--;
    if (buffer_count == BUF_SIZE-1){
        //cout << "Some Producer Wakeup"  << "\n";
        full.signal(); // wakeup waiting producer
    }
}

void check_store(Future<bool> * wait){
    if(buffer_count >= BUF_SIZE) // checking whether the buffer IS full
        wait->resolve(true);
    else
        wait->resolve(false);
}

void store(char item)
{
    buffer[in] = item;
    cout << "P->" << item << "\t";
    total_count++;
    if (total_count > 10){
        cout << "\n";
        total_count = 0;
    }
    in = (in + 1) % BUF_SIZE;
    buffer_count++;
    if (buffer_count == 1){
        //cout << "Some Consumer Wakeup"  << "\n";
        empty.signal(); // wakeup waiting consumer
    }
}

int consumer()
{
    for(int i = 0; i < iterations; i++) {
        Future<char> item{};
        Future<bool> wait{};
        buffer_guard.submit(check_load, &wait); // checking whether wait is necessary
        if (wait.get_value()){
            empty.wait(); // wait somehow
        }
        buffer_guard.submit(load, &item); // load item from buffer
        item.get_value();  // consume - part1
        unsigned long int delay_rand = Random::random();
        unsigned long int true_rand = (delay_rand > 9999) ? delay_rand / 10000 : delay_rand;
        Alarm::delay(true_rand/5); // consume - part3
    }
    return 0;
}

int producer()
{
    for(int i = 0; i < iterations; i++) {
        unsigned long int delay_rand = Random::random();
        unsigned long int true_rand = (delay_rand > 9999) ? delay_rand / 1000 : delay_rand;
        Alarm::delay(true_rand/20); // consume - part3
        char item = 'a' + in;  // produce - part2
        Future<bool> wait{};
        buffer_guard.submit(check_store, &wait); // checking whether wait is necessary
        if(wait.get_value()){
            //cout << "Producer Waiting"  << "\n";
            full.wait();
            //cout << "Producer Awaking"  << "\n";
        }
        buffer_guard.submit(store, item); // store item on buffer
    }
    return 0;
}

int main()
{
    cout << "Main started!" << endl;

    unsigned long int ini = clock.now();
	Random::seed(ini);

    Thread * prod = new Thread(&producer);
    Thread * cons = new Thread(&consumer);

    cons->join();
    prod->join();

    cout << "The end!" << endl;

    delete cons;
    delete prod;

    return 0;
}
