#ifndef FLOCKING_H
#define FLOCKING_H

#include <exec/types.h>

/* 
 * Definice hranic našeho podmořského světa pro hejno.
 */
#define FLOCK_BOUND_LEFT   36
#define FLOCK_BOUND_RIGHT  604
#define FLOCK_BOUND_TOP    22
#define FLOCK_BOUND_BOTTOM 232

/* 
 * Struktura pro jednu rybu v hejnu.
 * Používáme WORD (16-bit) pro rychlost na 68000.
 */
struct Fish; // Forward deklarace

/* 
 * Hlavní funkce pro aktualizaci pohybu hejna.
 * Aplikuje pravidla separace, soudržnosti, zarovnání a útěku před žralokem.
 */
void update_flocking(struct Fish *fish_array, int num_fish, WORD shark_x, WORD shark_y);

#endif
