#ifndef __PBC_UTIL_H__
#define __PBC_UTIL_H__

#define inline
// from the ACE project: http://www.cs.wustl.edu/~schmidt/ACE.html
// silences warnings, and generates no code for many compilers
// See ACE_wrappers/ace/ace/config-macros.h:391
#define UNUSED_VAR(a) do { /* nothing */ } while (&a == 0)

// I was using this before (and without macros):
//#define UNUSED_VAR(a) (void) a

//for storing small integers in void *
//see http://www.gelato.unsw.edu.au/archives/linux-ia64/0008/0344.html
static inline void *int_to_voidp(int i)
{
    //TODO: this won't work on some platforms 
    //assert(sizeof(long) == sizeof(void *));
    return (void *) (long) i;
}

#ifdef WIN32

int snprintf(char* buffer, size_t n, const char* format, ...);

#endif

#endif //__PBC_UTIL_H__
