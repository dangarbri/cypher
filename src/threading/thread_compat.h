#ifndef _THREADS_H
#define _THREADS_H

#include <time.h>

typedef unsigned int _Thrd_id_t;

struct thrd_t { // thread identifier for Win32
    void* _Hnd; // Win32 HANDLE
    _Thrd_id_t _Id;
};
typedef struct thrd_t thrd_t;

enum thrd_result { 
  thrd_success = 0,
  thrd_nomem = 1,
  thrd_timedout = 2,
  thrd_busy = 3,
  thrd_error = 4
};

typedef int (*thrd_start_t)(void*);
extern int __cdecl thrd_create(thrd_t*, thrd_start_t, void*);
extern int __cdecl thrd_detach(thrd_t);
extern int __cdecl thrd_join(thrd_t, int*);

#endif