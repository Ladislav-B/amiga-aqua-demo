#include <exec/types.h>
#include <graphics/gels.h>
#include "flocking.h"

/* 
 * Musíme znát strukturu Fish z main.c, 
 * ale abychom se vyhnuli cyklickým závislostem, 
 * definujeme ji zde lokálně tak, jak ji chceme v main.c mít.
 */
struct Fish {
    struct Bob *bob;
    WORD x, y;
    WORD dx, dy;
    WORD width;
};

/* 
 * Parametry hejna (Boids weights).
 * Na Amize 68000 budeme pracovat s malými celými čísly.
 */
#define DIST_MIN 20    /* Příliš blízko! (Separace) */
#define DIST_MAX 80    /* Dosah hejna (Soudržnost a Zarovnání) */

void update_flocking(struct Fish *fish_array, int num_fish) {
    int i, j;
    WORD dist_x, dist_y, dist_sq;
    WORD move_x, move_y;
    WORD avg_x, avg_y, avg_dx, avg_dy, neighbors;

    for (i = 0; i < num_fish; i++) {
        move_x = 0; move_y = 0;
        avg_x = 0; avg_y = 0;
        avg_dx = 0; avg_dy = 0;
        neighbors = 0;

        for (j = 0; j < num_fish; j++) {
            if (i == j) continue;

            dist_x = fish_array[i].x - fish_array[j].x;
            dist_y = fish_array[i].y - fish_array[j].y;
            
            /* Rychlý odhad vzdálenosti (čtverec) */
            dist_sq = (dist_x * dist_x + dist_y * dist_y);

            /* 1. SEPARACE (Separation) - Udržuj rozestup! */
            if (dist_sq < (DIST_MIN * DIST_MIN) && dist_sq > 0) {
                move_x += dist_x;
                move_y += dist_y;
            }
            /* 2. SOUDRŽNOST & 3. ZAROVNÁNÍ (Cohesion & Alignment) - Sousedé v dosahu */
            else if (dist_sq < (DIST_MAX * DIST_MAX)) {
                avg_x += fish_array[j].x;
                avg_y += fish_array[j].y;
                avg_dx += fish_array[j].dx;
                avg_dy += fish_array[j].dy;
                neighbors++;
            }
        }

        if (neighbors > 0) {
            /* Soudržnost - toulání se ke středu sousedů */
            avg_x = (avg_x / neighbors);
            avg_y = (avg_y / neighbors);
            move_x += (avg_x - fish_array[i].x) / 8;
            move_y += (avg_y - fish_array[i].y) / 8;

            /* Zarovnání - sjednocení rychlosti se sousedy */
            avg_dx = (avg_dx / neighbors);
            avg_dy = (avg_dy / neighbors);
            move_x += (avg_dx - fish_array[i].dx) / 4;
            move_y += (avg_dy - fish_array[i].dy) / 4;
        }

        /* Aplikace sil na rychlost ryby */
        fish_array[i].dx += move_x / 4;
        fish_array[i].dy += move_y / 4;

        /* Omezíme maximální rychlost (Speed Limiter) */
        if (fish_array[i].dx >  4) fish_array[i].dx =  4;
        if (fish_array[i].dx < -4) fish_array[i].dx = -4;
        if (fish_array[i].dy >  2) fish_array[i].dy =  2;
        if (fish_array[i].dy < -2) fish_array[i].dy = -2;

        /* Hranice akvária (Bouncing logic) */
        if (fish_array[i].x < FLOCK_BOUND_LEFT)   fish_array[i].dx += 1;
        if (fish_array[i].x > FLOCK_BOUND_RIGHT)  fish_array[i].dx -= 1;
        if (fish_array[i].y < FLOCK_BOUND_TOP)    fish_array[i].dy += 1;
        if (fish_array[i].y > FLOCK_BOUND_BOTTOM) fish_array[i].dy -= 1;

        /* Finální pohyb */
        fish_array[i].x += fish_array[i].dx;
        fish_array[i].y += fish_array[i].dy;
    }
}
