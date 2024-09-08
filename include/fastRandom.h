#ifndef _FAST_RANDOM_H_
#define _FAST_RANDOM_H_

#include "stdtypes.h"

void FastRandomSeed(int i);
int FastRand(int s, int e);
int FastRndOr(int s, int e);
float FastRnd();

#endif // _FAST_RANDOM_H_