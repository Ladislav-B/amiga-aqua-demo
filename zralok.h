#ifndef ZRALOK_H
#define ZRALOK_H

#include <exec/types.h>
#include <graphics/gels.h>

struct Shark {
    struct Bob *bob;
    WORD x, y;
    WORD dx, dy;
    WORD width;
    WORD height;
    WORD mask_size;
    
    /* Ukazatele na data grafik */
    APTR img_left;
    APTR mask_left;
    APTR img_right;
    APTR mask_right;
};

/* Inicializace žraloka */
BOOL init_shark(struct Shark *shark, struct RastPort *rp);

/* Pohyb žraloka a rozhánění ryb */
void update_shark(struct Shark *shark, struct RastPort *rp);

/* Úklid žraloka */
void cleanup_shark(struct Shark *shark);

#endif
