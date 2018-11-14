// EPOS Debug Utility Declarations

#ifndef __debug_sync_h
#define __debug_sync_h


#include <utility/debug.h>
#include <semaphore.h>

// You can define it anywhere before including this file
// #define DEBUG_SYNC

#ifdef DEBUG_SYNC
__BEGIN_UTIL
    Semaphore _debug_syncronized_semaphore_lock;
__END_UTIL

    // A debug function cannot call this recursively, otherwise a deadlock happens
    #define LOG(name,level,...) do { \
        _debug_syncronized_semaphore_lock.lock(); \
            db<name>(level) << __VA_ARGS__; \
        _debug_syncronized_semaphore_lock.unlock(); } while(0);

#else
    #define LOG(name,level,...) db<name>(level) << __VA_ARGS__;

#endif


#endif

