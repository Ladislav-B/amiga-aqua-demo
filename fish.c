#include <proto/exec.h>
#include <proto/graphics.h>
#include <graphics/gels.h>
#include <exec/memory.h>
#include "fish.h"
#include "ryba_levy_sprite.h" /* g77 (doleva) */
#include "ryba_pravy_sprite.h" /* g88 (doprava) */
#include "animtools/animtools.h"
#include "animtools/animtools_proto.h"
#include "random.h"

#define FISH_BOUND_LEFT   36
#define FISH_BOUND_RIGHT  604
#define FISH_BOUND_TOP    22
#define FISH_BOUND_BOTTOM 232

/* Velikost jedné roviny (planu) 32x32 px = 128 bajtů */
#define PLANE_SIZE 128

BOOL init_fish_squad(struct Fish *fArr, struct RastPort *rp) {
    NEWBOB fTemp = { NULL, 2, 32, 3, 0x07, 0x08, OVERLAY, 0, 4, 0, 0, 0, 0 };
    
    for (int i = 0; i < NUM_FISH; i++) {
        fArr[i].width = 32; fArr[i].height = 32;
        fArr[i].x = FISH_BOUND_LEFT + randRange(FISH_BOUND_RIGHT - FISH_BOUND_LEFT - 32);
        fArr[i].y = FISH_BOUND_TOP + randRange(FISH_BOUND_BOTTOM - FISH_BOUND_TOP - 32);
        fArr[i].dx = (randRange(10) > 5) ? 2 : -2;
        fArr[i].dy = (randRange(10) > 5) ? 1 : -1;

        /* Inicializace pomocí g77 */
        fTemp.nb_Image = (WORD *)g77_planes;
        fTemp.nb_X = fArr[i].x; fTemp.nb_Y = fArr[i].y;
        
        fArr[i].bob = makeBob(&fTemp);
        if (fArr[i].bob) {
            /* Správné nastavení počátečních dat (přeskočíme masku pro ImageData) */
            if (fArr[i].dx < 0) {
                fArr[i].bob->BobVSprite->ImageData = (WORD *)(g77_planes + PLANE_SIZE);
                CopyMem(g77_planes, fArr[i].bob->ImageShadow, PLANE_SIZE);
            } else {
                fArr[i].bob->BobVSprite->ImageData = (WORD *)(g88_planes + PLANE_SIZE);
                CopyMem(g88_planes, fArr[i].bob->ImageShadow, PLANE_SIZE);
            }
            AddBob(fArr[i].bob, rp);
        } else return FALSE;
    }
    return TRUE;
}

void update_fish_orientation(struct Fish *fArr) {
    for (int i = 0; i < NUM_FISH; i++) {
        WORD *tImg = (fArr[i].dx < 0) ? (WORD *)(g77_planes + PLANE_SIZE) : (WORD *)(g88_planes + PLANE_SIZE);
        UBYTE *tMsk = (fArr[i].dx < 0) ? g77_planes : g88_planes;

        if (fArr[i].bob->BobVSprite->ImageData != tImg) {
            fArr[i].bob->BobVSprite->ImageData = tImg;
            CopyMem(tMsk, fArr[i].bob->ImageShadow, PLANE_SIZE);
        }
        fArr[i].bob->BobVSprite->X = fArr[i].x;
        fArr[i].bob->BobVSprite->Y = fArr[i].y;
    }
}

void cleanup_fish_squad(struct Fish *fArr) {
    for (int i = 0; i < NUM_FISH; i++) {
        if (fArr[i].bob) { RemBob(fArr[i].bob); freeBob(fArr[i].bob, 4); }
    }
}
