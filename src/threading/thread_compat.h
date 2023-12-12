#ifdef _MSC_VER
// Visual Studio 2022 Version 17.8 claims to support the C11 threads standard
// but they did not ship a header file with it.
// However, the functions exist and can be linked to, so this file exposes
// some of the available features.
#ifndef _THREADS_H_
#define _THREADS_H_

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

#endif // _THREADS_H_

#else // _MSC_VER
#include <threads.h>
#endif // _MSC_VER