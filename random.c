#include "random.h"

static ULONG seed = 12345;  // Počáteční hodnota

void srand(ULONG newSeed) {
    seed = newSeed;
}

// Generuje náhodné číslo v rozsahu 0 - 32767
UWORD rand() {
    seed = (seed * 1664525L + 1013904223L);  
    return (seed >> 16) & 0xFFFF;
}

// Náhodné číslo v rozsahu 0 - max-1
UWORD randRange(UWORD max) {
    if (max == 0) return 0;
    return rand() % max;
}
