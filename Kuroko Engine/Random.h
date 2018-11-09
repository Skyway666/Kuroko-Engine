#ifndef __RNG
#define __RNG

#include "PCG Random/pcg_basic.h"

int randomFromTo(int n1, int n2);

float randomFloat();

uint32_t random32bits();

void randomizeSeed();


#endif