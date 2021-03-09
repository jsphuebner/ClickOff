#ifndef WILDCARDS_H
#define WILDCARDS_H

extern int wildcardfit (const char *wildcard, const char *test);
/* this function implements the UN*X wildcards and returns  */
/* 0  if *wildcard does not match *test                     */
/* 1  if *wildcard matches *test                            */

#endif


