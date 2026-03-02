#ifndef BUBBLE_SPRITE_H
#define BUBBLE_SPRITE_H

#include <exec/types.h>

/* Metadata for a 3-bitplane BOB */
#define BUBBLE_WIDTH 16
#define BUBBLE_HEIGHT 16
#define BUBBLE_DEPTH 3
#define BUBBLE_COLORS 8

/*
  Sprite data for a 16x16 bubble.
  Colors used from background palette:
  - 0 (000): Transparent
  - 7 (111): Border (Dark Blue from palette index 7)
  - 4 (100): Body (Light Blue from palette index 4)
  - 5 (101): Highlight (Yellow from palette index 5)
*/
__chip UBYTE bubble_planes[] = {
    // Plane 0 (for colors 1,3,5,7)
    0x00, 0x00, // row 0
    0x00, 0x00, // row 1
    0x00, 0x00, // row 2
    0x00, 0x00, // row 3
    0x03, 0xC0, // row 4: ..1111..
    0x04, 0x20, // row 5: .1....1.
    0x04, 0x60, // row 6: .1..11.1
    0x04, 0x20, // row 7: .1....1.
    0x03, 0xC0, // row 8: ..1111..
    0x00, 0x00, // row 9
    0x00, 0x00, // row 10
    0x00, 0x00, // row 11
    0x00, 0x00, // row 12
    0x00, 0x00, // row 13
    0x00, 0x00, // row 14
    0x00, 0x00, // row 15
    // Plane 1 (for colors 2,3,6,7)
    0x00, 0x00, // row 0
    0x00, 0x00, // row 1
    0x00, 0x00, // row 2
    0x00, 0x00, // row 3
    0x03, 0xC0, // row 4: ..1111..
    0x04, 0x20, // row 5: .1....1.
    0x04, 0x20, // row 6: .1....1.
    0x04, 0x20, // row 7: .1....1.
    0x03, 0xC0, // row 8: ..1111..
    0x00, 0x00, // row 9
    0x00, 0x00, // row 10
    0x00, 0x00, // row 11
    0x00, 0x00, // row 12
    0x00, 0x00, // row 13
    0x00, 0x00, // row 14
    0x00, 0x00, // row 15
    // Plane 2 (for colors 4,5,6,7)
    0x00, 0x00, // row 0
    0x00, 0x00, // row 1
    0x00, 0x00, // row 2
    0x00, 0x00, // row 3
    0x03, 0xC0, // row 4: ..1111..
    0x07, 0xE0, // row 5: .111111.
    0x07, 0xE0, // row 6: .111111.
    0x07, 0xE0, // row 7: .111111.
    0x03, 0xC0, // row 8: ..1111..
    0x00, 0x00, // row 9
    0x00, 0x00, // row 10
    0x00, 0x00, // row 11
    0x00, 0x00, // row 12
    0x00, 0x00, // row 13
    0x00, 0x00, // row 14
    0x00, 0x00  // row 15
};

#endif /* BUBBLE_SPRITE_H */