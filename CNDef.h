#ifndef CNDEF_H_H
#define CNDEF_H_H

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#if defined __GNUC__
#define likely(x) __builtin_expect ((x),1)
#define unlikely(x) __builtin_expect((x),0)
#else
#define likely(x)
#define unlikely(x)
#endif

#define NSBEGIN namespace CETCNAV{
#define NSEND }
#define USECETCNAV using namespace CETCNAV;

#endif
