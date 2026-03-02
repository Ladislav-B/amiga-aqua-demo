#ifndef RANDOM_H
#define RANDOM_H

#include <exec/types.h>

void srand(ULONG seed);
UWORD rand();
UWORD randRange(UWORD max);

#endif /* RANDOM_H */
