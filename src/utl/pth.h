//! \file thr.h \brief threads

#include <unistd.h>
#include <pthread.h>

#ifdef __APPLE__
#include <mach/mach_traps.h>
#include <mach/semaphore.h>
#else
#include <semaphore.h>
#endif

#define __thread