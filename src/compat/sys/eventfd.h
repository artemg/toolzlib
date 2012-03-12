#ifndef _COMPAT_SYS_EVENTFD
#define _COMPAT_SYS_EVENTFD

#include <unistd.h>
#include <syscall.h>

/* Flags for signalfd.  */
enum
{
    EFD_SEMAPHORE = 1,
#define EFD_SEMAPHORE EFD_SEMAPHORE
    EFD_CLOEXEC = 02000000,
#define EFD_CLOEXEC EFD_CLOEXEC
    EFD_NONBLOCK = 04000
#define EFD_NONBLOCK EFD_NONBLOCK
};

#ifndef __NR_eventfd2
#if defined(__x86_64__)
#define __NR_eventfd2 290
#elif defined(__i386__)
#define __NR_eventfd2 328
#else
#error Cannot detect your architecture!
#endif
#endif

static inline int eventfd (int count, int flags)
{
    return syscall(__NR_eventfd2, count, flags);
}

#endif

