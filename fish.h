#ifndef FISH_H
#define FISH_H

#include <exec/types.h>
#include <graphics/gels.h>

#define NUM_FISH 12

struct Fish {
    struct Bob *bob;
    WORD x, y;
    WORD dx, dy;
    WORD width;
    WORD height;
};

/* Inicializace hejna používající g77 a g88 */
BOOL init_fish_squad(struct Fish *fish_array, struct RastPort *rp);

/* Aktualizace pozice a směru (přepínání mezi g77 a g88) */
void update_fish_orientation(struct Fish *fish_array);

/* Úklid Bobů */
void cleanup_fish_squad(struct Fish *fish_array);

#endif
