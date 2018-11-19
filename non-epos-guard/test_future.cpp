#include <cassert>
#include <iostream>
#include <chrono>

#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

std::recursive_mutex _debug_syncronized_semaphore_lock;

#define DB(...) do { \
        std::unique_lock<std::recursive_mutex> lock(_debug_syncronized_semaphore_lock); \
        std::cout << __VA_ARGS__ << std::flush; } while(0);

template<typename FutureType>
class Future
{
public:
    Future(): _size(0), _is_resolved(false) {
        DB( "Future(_is_resolved=" << _is_resolved
                << ", _condition=" << _size
                << ") => " << this << std::endl )
    }

    ~Future() {
        DB( "~Future(this=" << this << ")" << std::endl );
    }

    FutureType get_value() {
        DB( "Future::get_value(this=" << this
                << " _is_resolved=" << _is_resolved
                << " _condition=" << _size
                <<  ")" << std::endl )
        // Optimization once _is_resolved is set true, we do not need lock anymore
        if( _is_resolved ) {
            return _value;
        }

        // If _is_resolved is not set to true, lock and double check _is_resolved
        _lock.lock();
        if(!_is_resolved) {
            ++_size;
            std::unique_lock<std::mutex> lock(_mutex);

            // We cannot call _lock.unlock(); before _condition.wait(lock); because
            // 1. It would allow this thread to be preempted
            // 2. Then, some other thread could call resolve()
            // Once this other thread completes the resolve() call, and this
            // thread is rescheduled, we would finally call _condition.wait(lock);
            // but doing so, would cause this thread to be locked indefinitely
            // because we will never call resolve() anymore
            _condition.wait(lock); // How to call _lock.unlock(); after locking?
        }
        else {
            _lock.unlock();
        }
        return _value;
    }

    void resolve(FutureType value) {
        DB( "Future::resolve(this=" << this
                << " _is_resolved=" << _is_resolved
                << " _condition=" << _size
                <<  ")" << std::endl )
        _lock.lock();
        assert(!_is_resolved);
        // If the instruction pointer got until here, and the thread is unscheduled,
        // and another thread call `resolve()`, then, the `assert` will not work,
        // if the whole resolve() call is not atomic.
        _value = value;
        _is_resolved = true;
        _lock.unlock();
        _condition.notify_all();
    }

private:
    FutureType _value;
    std::atomic<int> _size;
    volatile std::atomic<bool> _is_resolved;

    std::mutex _mutex;
    std::recursive_mutex _lock;
    std::condition_variable _condition;
};


int producerFunction(Future<int>* future) {
    DB( "producerFunction ()" << std::endl )

    std::this_thread::sleep_for( std::chrono::milliseconds(2000) );
    future->resolve(10);

    DB( "producerFunction (resolving future=" << future << " to 10)" << std::endl )
    return 0;
}

int consumerFunction(Future<int>* future) {
    DB( "consumerFunction ()" << std::endl )

    auto value = future->get_value();
    DB( "consumerFunction (result=" << value << ")" << std::endl )

    value = future->get_value();
    DB( "consumerFunction (result=" << value << ")" << std::endl )
    return 0;
}

int main()
{
    DB( "Starting main application..." << std::endl )
    Future<int>* future = new Future<int>();

    std::thread* consumer = new std::thread(&consumerFunction, future);
    std::thread* producer = new std::thread(&producerFunction, future);

    consumer->join();
    producer->join();

    DB( "Exiting main application..." << std::endl )
    return 0;
}

