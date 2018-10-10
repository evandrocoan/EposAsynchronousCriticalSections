// EPOS Synchronizer Component Test Program

#include <utility/ostream.h>
#include <thread.h>
#include <semaphore.h>
#include <alarm.h>
#include <cpu.h>

using namespace EPOS;

int iterations = 10000000;
int const_iter = iterations;
int count = 0;

OStream cout;

int add(){
    while(iterations > 0){
        count++;
        iterations--;
    }
    return 0;
}

int main()
{
    cout << "Program Started" << endl;

    Thread * adders[5];

    for(int i = 0; i < 5; i++)
        adders[i] = new Thread(&add);

    cout << "Main Thread is now waiting" << endl;
    for(int i = 0; i < 5; i++) {
        int ret = adders[i]->join();
        cout << "Thread " << i << " finished and returned " << ret  << endl;
    }

    cout << "All Threads Have Finished" << endl;
    cout << "The counter is        : " << count << endl;
    cout << "The counter should be : " << const_iter << endl;

    for(int i = 0; i < 5; i++)
        delete adders[i];

    cout << "The end!" << endl;

    return 0;
}