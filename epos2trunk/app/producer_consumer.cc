// EPOS Synchronizer Component Test Program

#include <utility/ostream.h>
#include <thread.h>
#include <semaphore.h>
#include <alarm.h>
#include <guard.h>

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
    for(int i = 0; i < iterations; i++) {
        buffer_guard.submit(new Critical_Section<int>(&consume, new Future<int>()));
        Alarm::delay(5000);
    }
    return 0;
}

int main()
{
    Thread * cons = new Thread(&consumer);

    // producer
    for(int i = 0; i < iterations; i++) {
        Alarm::delay(5000);
        buffer_guard.submit(new Critical_Section<int>(&produce, new Future<int>()));
    }

    cons->join();

    cout << "The end!" << endl;

    delete cons;

    return 0;
}