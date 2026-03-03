/*
Vykreslí podmořský svět s hlubokomořským gradientem, inteligentním hejnem ryb a žralokem.
vc +aos68k .\main.c more_copper.c flocking.c fish.c zralok.c animtools/animtools.c random.c -o a.exe -lamiga
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gels.h>
#include <hardware/custom.h>

#include "background.h"
#include "random.h"
#include "bubble_sprite.h"
#include "animtools/animtools.h"
#include "animtools/animtools_proto.h"
#include "more_copper.h"
#include "flocking.h"
#include "fish.h"
#include "zralok.h"

struct IntuitionBase *IntuitionBase=NULL;
struct DosBase *DosBase=NULL;
struct GfxBase *GfxBase=NULL;

struct DoubleBuffer {
    struct Screen *screen;
    struct BitMap *bitmaps[2];
    struct BitMap *bgBitMap;
    int frame;
} *db;

volatile struct Custom *custom_ptr = (struct Custom *)0xDFF000;

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 256
#define SCREEN_DEPTH 4

#define BOUND_LEFT   36
#define BOUND_RIGHT  604
#define BOUND_TOP    22
#define BOUND_BOTTOM 232

#define NUM_BUBBLES 20
struct Bubble { struct Bob *bob; WORD x, y, dy; } bubbles[NUM_BUBBLES];

struct Fish fish[NUM_FISH];
struct Shark shark;

struct GelsInfo *gelsInfo;
UWORD pens[]={~0};
UWORD myPalette[16] = {
    0x0223,0x0BC9,0x0B75,0x0289,0x0556,0x0CB7,0x06A9,0x0246,
    0x0000,0x0820,0x0B30,0x0D40,0x0F60,0x0FB0,0x0F91,0x0FFF
};

struct TagItem screenTags[] = {
    SA_Width,SCREEN_WIDTH, SA_Height,SCREEN_HEIGHT, SA_Depth,SCREEN_DEPTH,
    SA_LikeWorkbench, FALSE, SA_DisplayID, HIRES_KEY,
    SA_Type,CUSTOMSCREEN|SCREENHIRES|CUSTOMBITMAP,
    SA_Colors,NULL, SA_Pens,(ULONG)pens, SA_SharePens,TRUE, SA_ShowTitle, FALSE,
    TAG_DONE, NULL
};

int main(void) {
  db = AllocVec(sizeof(struct DoubleBuffer), MEMF_CLEAR | MEMF_PUBLIC);
  if (db) {
    DosBase = (struct DosBase *)OpenLibrary("dos.library", 40);
    IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 40);
    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 40);

    if (GfxBase && IntuitionBase) {
      db->screen = (struct Screen*)OpenScreenTagList(NULL, (struct TagItem *)&screenTags);
      if (db->screen) {
        db->bitmaps[0] = db->screen->ViewPort.RasInfo->BitMap;
        db->bitmaps[1] = AllocBitMap(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, BMF_DISPLAYABLE|BMF_INTERLEAVED, db->bitmaps[0]);
        db->bgBitMap = AllocBitMap(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, BMF_DISPLAYABLE|BMF_INTERLEAVED, db->bitmaps[0]);

        if (db->bitmaps[1] && db->bgBitMap) {
          LoadRGB4(&db->screen->ViewPort, (UWORD *)myPalette, 16);
          struct RastPort bgRp; InitRastPort(&bgRp); bgRp.BitMap = db->bgBitMap;
          DrawImage(&bgRp, &background, 0, 0); WaitBlit();

          if ((gelsInfo = setupGelSys(&db->screen->RastPort, 0x03))) {
            // --- Copper Gradient ---
            struct UCopList *ucl = create_gradient();
            if (ucl) { db->screen->ViewPort.UCopIns = ucl; }

            // --- Bubliny ---
            NEWBOB bTemp = { (WORD *)bubble_planes, BUBBLE_WIDTH/16, BUBBLE_HEIGHT, BUBBLE_DEPTH, 0x07, 0, OVERLAY, 0, SCREEN_DEPTH, 0, 0, 0, 0 };
            for (int i=0; i<NUM_BUBBLES; i++) {
              bubbles[i].x = BOUND_LEFT + randRange(BOUND_RIGHT - BOUND_LEFT - BUBBLE_WIDTH);
              bubbles[i].y = BOUND_TOP + randRange(BOUND_BOTTOM - BOUND_TOP);
              bubbles[i].dy = -(1 + randRange(3));
              bTemp.nb_X = bubbles[i].x; bTemp.nb_Y = bubbles[i].y;
              bubbles[i].bob = makeBob(&bTemp); if (bubbles[i].bob) AddBob(bubbles[i].bob, &db->screen->RastPort);
            }

            // --- Ryby (S inteligentním hejnem) ---
            init_fish_squad(fish, &db->screen->RastPort);
            
            // --- ŽRALOK (Náš nový predátor) ---
            init_shark(&shark, &db->screen->RastPort);

            MakeScreen(db->screen); RethinkDisplay();

            int front=0;
            while ((*((volatile UBYTE *)0xbfe001) & 0x40)) {
              int back = 1 - front;
              struct RastPort rp; InitRastPort(&rp); rp.BitMap = db->bitmaps[back]; rp.GelsInfo = gelsInfo;

              BltBitMap(db->bgBitMap, 0, 0, db->bitmaps[back], 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xC0, 0xFF, NULL);
              WaitBlit();

              // 1. Bubliny
              for (int j=0; j<NUM_BUBBLES; j++) {
                bubbles[j].y += bubbles[j].dy; bubbles[j].x += randRange(5)-2;
                if (bubbles[j].y < BOUND_TOP) { bubbles[j].y = BOUND_BOTTOM-5; bubbles[j].x = BOUND_LEFT + randRange(BOUND_RIGHT-BOUND_LEFT-BUBBLE_WIDTH); }
                bubbles[j].bob->BobVSprite->X = bubbles[j].x; bubbles[j].bob->BobVSprite->Y = bubbles[j].y;
              }
              
              // 2. Žralok
              update_shark(&shark, &db->screen->RastPort);

              // 3. Hejno ryb (Reagují na žraloka!)
              update_flocking(fish, NUM_FISH, shark.x, shark.y);
              update_fish_orientation(fish);

              SortGList(&db->screen->RastPort); DrawGList(&rp, &db->screen->ViewPort);
              BltBitMap(db->bgBitMap, 0, 0, db->bitmaps[back], 0, 0, SCREEN_WIDTH, BOUND_TOP, 0xC0, 0xFF, NULL);
              BltBitMap(db->bgBitMap, 0, BOUND_BOTTOM, db->bitmaps[back], 0, BOUND_BOTTOM, SCREEN_WIDTH, SCREEN_HEIGHT-BOUND_BOTTOM, 0xC0, 0xFF, NULL);
              BltBitMap(db->bgBitMap, 0, BOUND_TOP, db->bitmaps[back], 0, BOUND_TOP, BOUND_LEFT, BOUND_BOTTOM - BOUND_TOP, 0xC0, 0xFF, NULL);
              BltBitMap(db->bgBitMap, BOUND_RIGHT, BOUND_TOP, db->bitmaps[back], BOUND_RIGHT, BOUND_TOP, SCREEN_WIDTH - BOUND_RIGHT, BOUND_BOTTOM - BOUND_TOP, 0xC0, 0xFF, NULL);
              WaitBlit();

              WaitTOF();
              db->screen->ViewPort.RasInfo->BitMap = db->bitmaps[back];
              front = back;
              ScrollVPort(&db->screen->ViewPort);
            }

            cleanup_shark(&shark);
            cleanup_fish_squad(fish);
            for (int i = 0; i < NUM_BUBBLES; i++) { if (bubbles[i].bob) { RemBob(bubbles[i].bob); freeBob(bubbles[i].bob, SCREEN_DEPTH); } }
            cleanupGelSys(gelsInfo, &db->screen->RastPort);
            if (db->screen->ViewPort.UCopIns) { FreeVPortCopLists(&db->screen->ViewPort); FreeVec(db->screen->ViewPort.UCopIns); db->screen->ViewPort.UCopIns=NULL; }
          }
          if (db->bgBitMap) FreeBitMap(db->bgBitMap);
          FreeBitMap(db->bitmaps[1]);
        }
        CloseScreen(db->screen);
      }
    }
    if (GfxBase) CloseLibrary((struct Library *)GfxBase);
    if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
    if (DosBase) CloseLibrary((struct Library *)DosBase);
    FreeVec(db);
  }
  return 0;
}
