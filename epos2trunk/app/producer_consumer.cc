// EPOS Synchronizer Component Test Program

#include <utility/ostream.h>
#include <thread.h>
#include <semaphore.h>
#include <alarm.h>
#include <guard.h>
#include <future.h>

using namespace EPOS;

const int iterations = 100;

OStream cout;

Guard buffer_guard;
const int BUF_SIZE = 16;
char buffer[BUF_SIZE];
int buffer_count = 0;
int out = 0;
int in = 0;

void consume()
{
    cout << "C<-" << buffer[out] << "\t";
    out = (out + 1) % BUF_SIZE;
    buffer_count--;
}

void produce()
{
    buffer[in] = 'a' + in;
    buffer_count++;
    cout << "P->" << buffer[in] << "\t";
    in = (in + 1) % BUF_SIZE;
}

int consumer()
{
    for(int index = 0; index < iterations; index++) {
        buffer_guard.submit(&consume);
        Alarm::delay(5000);
    }
    return 0;
}

int producer()
{
    for(int index = 0; index < iterations; index++) {
        Alarm::delay(5000);
        buffer_guard.submit(&produce);
    }
    return 0;
}

int main()
{
    Thread * consumer = new Thread(&consumer);
    Thread * producer = new Thread(&producer);

    consumer->join();
    producer->join();

    cout << "The end!" << endl;

    delete consumer;
    delete producer;

    return 0;
}

// // EPOS Synchronizer Component Test Program

// #include <utility/ostream.h>
// #include <thread.h>
// #include <semaphore.h>
// #include <alarm.h>

// using namespace EPOS;

// const int iterations = 100;

// OStream cout;

// const int BUF_SIZE = 16;
// char buffer[BUF_SIZE];
// Semaphore empty(BUF_SIZE);
// Semaphore full(0);

// int consumer()
// {
//     int out = 0;
//     for(int i = 0; i < iterations; i++) {
//         full.p();
//         cout << "C<-" << buffer[out] << "\t";
//         out = (out + 1) % BUF_SIZE;
//         Alarm::delay(5000);
//         empty.v();
//     }

//     return 0;
// }

// int main()
// {
//     Thread * cons = new Thread(&consumer);

//     // producer
//     int in = 0;
//     for(int i = 0; i < iterations; i++) {
//         empty.p();
//         Alarm::delay(5000);
//         buffer[in] = 'a' + in;
//         cout << "P->" << buffer[in] << "\t";
//         in = (in + 1) % BUF_SIZE;
//         full.v();
//     }

//     cons->join();

//     cout << "The end!" << endl;

//     delete cons;

//     return 0;
// }
