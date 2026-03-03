#include <exec/types.h>
#include <graphics/gels.h>
#include "flocking.h"

struct Fish {
    struct Bob *bob;
    WORD x, y;
    WORD dx, dy;
    WORD width;
    WORD height;
};

#define DIST_MIN 25
#define DIST_MAX 100

void update_flocking(struct Fish *fArr, int num_fish) {
    int i, j;
    LONG dist_x, dist_y, dist_sq; /* Musí být LONG! */
    WORD move_x, move_y;
    LONG avg_x, avg_y, avg_dx, avg_dy;
    WORD neighbors;

    for (i = 0; i < num_fish; i++) {
        move_x = 0; move_y = 0;
        avg_x = 0; avg_y = 0;
        avg_dx = 0; avg_dy = 0;
        neighbors = 0;

        for (j = 0; j < num_fish; j++) {
            if (i == j) continue;

            dist_x = (LONG)fArr[i].x - fArr[j].x;
            dist_y = (LONG)fArr[i].y - fArr[j].y;
            dist_sq = (dist_x * dist_x) + (dist_y * dist_y);

            /* 1. SEPARACE */
            if (dist_sq < (LONG)DIST_MIN * DIST_MIN && dist_sq > 0) {
                move_x += (WORD)dist_x;
                move_y += (WORD)dist_y;
            }
            /* 2. SOUDRŽNOST & ZAROVNÁNÍ */
            else if (dist_sq < (LONG)DIST_MAX * DIST_MAX) {
                avg_x += fArr[j].x;
                avg_y += fArr[j].y;
                avg_dx += fArr[j].dx;
                avg_dy += fArr[j].dy;
                neighbors++;
            }
        }

        if (neighbors > 0) {
            avg_x /= neighbors;
            avg_y /= neighbors;
            move_x += (WORD)(avg_x - fArr[i].x) / 16;
            move_y += (WORD)(avg_y - fArr[i].y) / 16;

            avg_dx /= neighbors;
            avg_dy /= neighbors;
            move_x += (WORD)(avg_dx - fArr[i].dx) / 4;
            move_y += (WORD)(avg_dy - fArr[i].dy) / 4;
        }

        fArr[i].dx += move_x / 4;
        fArr[i].dy += move_y / 4;

        /* Přísné omezení rychlosti */
        if (fArr[i].dx >  3) fArr[i].dx =  3;
        if (fArr[i].dx < -3) fArr[i].dx = -3;
        if (fArr[i].dy >  1) fArr[i].dy =  1;
        if (fArr[i].dy < -1) fArr[i].dy = -1;

        /* Odraz od stěn */
        if (fArr[i].x < FLOCK_BOUND_LEFT)   fArr[i].dx = 2;
        if (fArr[i].x > FLOCK_BOUND_RIGHT)  fArr[i].dx = -2;
        if (fArr[i].y < FLOCK_BOUND_TOP)    fArr[i].dy = 1;
        if (fArr[i].y > FLOCK_BOUND_BOTTOM) fArr[i].dy = -1;

        fArr[i].x += fArr[i].dx;
        fArr[i].y += fArr[i].dy;
    }
}
