#include <proto/exec.h>
#include <proto/graphics.h>
#include <graphics/gels.h>
#include <exec/memory.h>
#include "zralok.h"
#include "zralok_levy.h"
#include "zralok_pravy.h"
#include "animtools/animtools.h"
#include "animtools/animtools_proto.h"
#include "random.h"

#define SHARK_BOUND_LEFT   30
#define SHARK_BOUND_RIGHT  540
#define SHARK_BOUND_TOP    30
#define SHARK_BOUND_BOTTOM 210

#define SHARK_WIDTH_WORDS 4
#define SHARK_HEIGHT      32
#define SHARK_PLANE_SIZE  (SHARK_WIDTH_WORDS * SHARK_HEIGHT * 2) /* 256 bajtů */
#define SHARK_MASK_SIZE   (SHARK_WIDTH_WORDS * SHARK_HEIGHT * 2) /* 256 bajtů */

static APTR prepare_shark_data(UWORD *source_data) {
    /* Alokujeme Masku (1 plane) + Obraz (3 plany) = 4 plany celkem */
    LONG total_size = SHARK_MASK_SIZE + (3 * SHARK_PLANE_SIZE);
    UWORD *dest = AllocVec(total_size, MEMF_CHIP | MEMF_CLEAR);
    
    if (dest) {
        UWORD *mask = dest;
        UWORD *p0 = dest + (SHARK_MASK_SIZE / 2);
        UWORD *p1 = p0 + (SHARK_PLANE_SIZE / 2);
        UWORD *p2 = p1 + (SHARK_PLANE_SIZE / 2);
        
        UWORD *src_p0 = source_data;
        UWORD *src_p1 = source_data + 128; /* 128 WORDů na plane */
        UWORD *src_p2 = source_data + 256;
        
        CopyMem(src_p0, p0, SHARK_PLANE_SIZE);
        CopyMem(src_p1, p1, SHARK_PLANE_SIZE);
        CopyMem(src_p2, p2, SHARK_PLANE_SIZE);
        
        /* Generujeme masku: Mask = P0 OR P1 OR P2 (průhlednost na barvě 0) */
        for (int i = 0; i < 128; i++) {
            mask[i] = src_p0[i] | src_p1[i] | src_p2[i];
        }
    }
    return (APTR)dest;
}

BOOL init_shark(struct Shark *shark, struct RastPort *rp) {
    /* nb_PlaneOnOff = 0x00, používáme barvy 0-7 */
    NEWBOB sTemp = { NULL, 4, 32, 3, 0x07, 0x00, OVERLAY, 0, 4, 0, 0, 0, 0 };
    
    shark->width = 64;
    shark->height = 32;
    shark->mask_size = SHARK_MASK_SIZE;
    
    shark->img_left = prepare_shark_data(zralok_levyData);
    shark->img_right = prepare_shark_data(zralok_pravyData);
    
    if (!shark->img_left || !shark->img_right) return FALSE;

    shark->x = 100;
    shark->y = 100;
    shark->dx = 2;
    shark->dy = 1;

    sTemp.nb_Image = (WORD *)shark->img_right;
    sTemp.nb_X = shark->x; sTemp.nb_Y = shark->y;
    
    shark->bob = makeBob(&sTemp);
    if (shark->bob) {
        /* ImageData musí ukazovat na barevné roviny (za masku) */
        shark->bob->BobVSprite->ImageData = (WORD *)((UBYTE *)shark->img_right + SHARK_MASK_SIZE);
        AddBob(shark->bob, rp);
        return TRUE;
    }
    return FALSE;
}

void update_shark(struct Shark *shark, struct RastPort *rp) {
    shark->x += shark->dx;
    shark->y += shark->dy;

    if (shark->x < SHARK_BOUND_LEFT || shark->x > SHARK_BOUND_RIGHT) shark->dx = -shark->dx;
    if (shark->y < SHARK_BOUND_TOP || shark->y > SHARK_BOUND_BOTTOM) shark->dy = -shark->dy;

    APTR tFull = (shark->dx < 0) ? shark->img_left : shark->img_right;
    APTR tImg  = (APTR)((UBYTE *)tFull + SHARK_MASK_SIZE);
    APTR tMsk  = tFull;

    if (shark->bob->BobVSprite->ImageData != (WORD *)tImg) {
        shark->bob->BobVSprite->ImageData = (WORD *)tImg;
        CopyMem(tMsk, shark->bob->ImageShadow, SHARK_MASK_SIZE);
    }

    shark->bob->BobVSprite->X = shark->x;
    shark->bob->BobVSprite->Y = shark->y;
}

void cleanup_shark(struct Shark *shark) {
    if (shark->bob) { RemBob(shark->bob); freeBob(shark->bob, 4); }
    if (shark->img_left) FreeVec(shark->img_left);
    if (shark->img_right) FreeVec(shark->img_right);
}
