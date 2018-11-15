#include <iostream>
#include "pthread.h"

#define ASM                     __asm__ __volatile__

using namespace std;

pthread_t pool[4];
static pthread_mutex_t mutex;

int counter = 0;
static const int iterations = 1e7;

void increment_counter(){ counter = counter + 1; }

void * mythread(void * id) {

    char arg = (char)((long)(id))+64;

    pthread_mutex_lock(&mutex);
    cout << "[" << arg << "] : begin (" << counter << ")" << endl;
    pthread_mutex_unlock(&mutex);

    for (int i = iterations; i > 0 ; i--) {
        counter = counter + 1;
    }

    pthread_mutex_lock(&mutex);
    cout << "[" << arg << "] : end   (" << counter << ")" << endl;
    pthread_mutex_unlock(&mutex);

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

//  g++ main.cpp -lpthread -m32

