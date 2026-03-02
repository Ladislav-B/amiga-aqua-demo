/*
Vykreslí obrázek podmořské základny a kolem budou plavat bubliny

vc +aos68k .\main.c animtools/animtools.c random.c -o a.exe -lamiga
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


/* Deklarace proměnných */
struct IntuitionBase *IntuitionBase=NULL;
struct DosBase *DosBase=NULL;
struct GfxBase *GfxBase=NULL;

// Naše struktura pro správu bufferů
struct DoubleBuffer {
    struct Screen *screen;      // Hlavní obrazovka
    struct BitMap *bitmaps[2];  // 0 = Front (viditelný), 1 = Back (kreslící)
    int frame;                  // Počítadlo snímků (pro přepínání 0 a 1)
} *db;

// Přístup k hardwaru Amigy
volatile struct Custom *custom = (struct Custom *)0xDFF000;

/* Size for screen */
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 256
#define SCREEN_DEPTH 3

// --- Nové struktury pro bubliny ---
#define NUM_BUBBLES 20

// Artefakt pro naši bublinu
struct Bubble {
    struct Bob *bob; // Ukazatel na BOB objekt
    WORD x, y;       // Aktuální pozice
    WORD dx, dy;         // Rychlost pohybu po ose Y
};

struct Bubble bubbles[NUM_BUBBLES];
struct GelsInfo *gelsInfo;
// --- Konec nových struktur ---


struct DrawInfo *drawInfo = NULL;

UWORD pens[]={~0};

struct TagItem screenTags[12] = {
        SA_Width,SCREEN_WIDTH,
        SA_Height,SCREEN_HEIGHT,
        SA_Depth,SCREEN_DEPTH,
        SA_LikeWorkbench, FALSE,
        SA_DisplayID, HIRES_KEY,
        SA_Title,(ULONG)"Underwater demo",
        SA_Type,CUSTOMSCREEN|SCREENHIRES|CUSTOMBITMAP,
        SA_Colors,NULL,
        SA_Pens,(ULONG)pens,
        SA_SharePens,TRUE,
        SA_ShowTitle, FALSE,
        TAG_DONE, NULL
};

int main(void) {
  db = AllocVec(sizeof(struct DoubleBuffer), MEMF_CLEAR | MEMF_PUBLIC);
  
  if (db) {
    DosBase = (struct DosBase *)OpenLibrary("dos.library", 40);
    if (DosBase) {
      IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 40);
      if (IntuitionBase) {
        GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 40);
        if (GfxBase) {
          db->screen = (struct Screen*)OpenScreenTagList(NULL, (struct TagItem *)&screenTags);
          if (db->screen) {
            // První bitmapu nám vytvořil systém s obrazovkou
            db->bitmaps[0] = db->screen->ViewPort.RasInfo->BitMap;

            // Alokujeme druhou (Back) bitmapu
            db->bitmaps[1] = AllocBitMap(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, BMF_DISPLAYABLE | BMF_INTERLEAVED, db->screen->ViewPort.RasInfo->BitMap);

            if (db->bitmaps[1]) {
              db->frame = 0; // Začínáme snímkem 0

              LoadRGB4(&db->screen->ViewPort, (UWORD *)backgroundPaletteRGB4, 8);
              
              // Vykreslíme pozadí do prvního (front) bufferu
              DrawImage(&db->screen->RastPort, &background, 0, 0);
              
              // Zkopírujeme pozadí i do druhého (back) bufferu ***
              WaitBlit(); // Počkáme na Blitter pro jistotu
              BltBitMap(db->bitmaps[0], 0, 0, 
                        db->bitmaps[1], 0, 0, 
                        SCREEN_WIDTH, SCREEN_HEIGHT, 
                        0xC0, 0xFF, NULL); // Minterm 0xC0 = kopírování (A->D)

              // --- Vyvolávací rituál pro bubliny ---
              srand(custom->vposr); // Inicializace náhody
              if ((gelsInfo = setupGelSys(&db->screen->RastPort, 0x03))) {
                  
                  // Recept na výrobu bubliny
                  NEWBOB newBobTemplate = {
                      (WORD *)bubble_planes,
                      BUBBLE_WIDTH / 16, // Šířka v jednotkách WORD (16 pixelů = 1 WORD)
                      BUBBLE_HEIGHT,
                      BUBBLE_DEPTH,
                      1 | 2 | 8, // PlanePick (použijeme všechny 3 roviny)
                      0, // PlaneOnOff
                      SAVEBACK | OVERLAY, // Flags - důležité pro obnovu pozadí
                      0, // Dbuf
                      SCREEN_DEPTH, // RasDepth
                      0, 0, // X, Y pozice (nastavíme v cyklu)
                      0, 0  // HitMask, MeMask
                  };

                  // Vyvoláme armádu 20 bublin
                  for (int i = 0; i < NUM_BUBBLES; i++) {
                      bubbles[i].x = randRange(SCREEN_WIDTH - BUBBLE_WIDTH)+ 80 - 40;
                      bubbles[i].y = SCREEN_HEIGHT / 2 + randRange(SCREEN_HEIGHT / 2) - 50;
                      bubbles[i].dx = 0; // kmitání
                      bubbles[i].dy = -(1 + randRange(4)); // Různá rychlost směrem nahoru

                      newBobTemplate.nb_X = bubbles[i].x;
                      newBobTemplate.nb_Y = bubbles[i].y;

                      if ((bubbles[i].bob = makeBob(&newBobTemplate))) {
                          AddBob(bubbles[i].bob, &db->screen->RastPort);
                      }
                  }
              
                  int frontBuffer = 0; // Který buffer je zobrazen (index)

                  for (UWORD i=200; i > 0; i--){
                    // --- Bitevní vřava - hlavní smyčka ---
                    int backBuffer = 1 - frontBuffer;
                    
                    // Připravíme si RastPort pro kreslení do zadního bufferu
                    struct RastPort rp;
                    InitRastPort(&rp);
                    rp.BitMap = db->bitmaps[backBuffer];
                    rp.GelsInfo = gelsInfo; // Propojíme Gels se správným RastPortem
                    
                    // 1. Vyčistíme zadní buffer překreslením pozadí
                    DrawImage(&rp, &background, 0, 0);

                    // 2. Oživíme bubliny
                    for (int j = 0; j < NUM_BUBBLES; j++) {
                        if (bubbles[j].bob) {
                            bubbles[j].dx =  randRange(8) - 4; // kmitání
                            bubbles[j].y += bubbles[j].dy;
                            bubbles[j].x += bubbles[j].dx;
                            // Když bublina opustí obrazovku nahoře
                            if (bubbles[j].y < BUBBLE_HEIGHT + 15) {
                                bubbles[j].y = SCREEN_HEIGHT - 50; // Vrať ji dolů
                                bubbles[j].x = randRange(SCREEN_WIDTH - BUBBLE_WIDTH) + 80 - 40; // na novou X pozici
                                bubbles[j].dy = -(1 + randRange(4)); // a s novou rychlostí
                            }
                            // Aktualizujeme pozici samotného BOBu
                            bubbles[j].bob->BobVSprite->X = bubbles[j].x;
                            bubbles[j].bob->BobVSprite->Y = bubbles[j].y;
                        }
                    }

                    // 3. Vykreslíme všechny BOB-y do zadního bufferu
                    SortGList(&db->screen->RastPort);
                    DrawGList(&rp, &db->screen->ViewPort);

                    // 4. SYNCHRONIZACE A SWAP
                    WaitTOF();
                    WaitBlit();

                    frontBuffer = 1 - frontBuffer;
                    db->screen->ViewPort.RasInfo->BitMap = db->bitmaps[frontBuffer];
                    
                    MakeScreen(db->screen);
                    RethinkDisplay();

                    db->frame++;
                  }

                  // --- Očistný rituál ---
                  for (int i = 0; i < NUM_BUBBLES; i++) {
                      if (bubbles[i].bob) {
                          RemBob(bubbles[i].bob);
                          freeBob(bubbles[i].bob, SCREEN_DEPTH); // *** OPRAVA: Uvolníme paměť pro BOBa ***
                      }
                  }
                  cleanupGelSys(gelsInfo, &db->screen->RastPort);
              }
              // --- Konec rituálů pro bubliny ---

              FreeBitMap(db->bitmaps[1]);
            }
            CloseScreen(db->screen);
          }
          CloseLibrary((struct Library *)GfxBase);
        }
        CloseLibrary((struct Library *)IntuitionBase);
      }
      CloseLibrary((struct Library *)DosBase);
    }
    FreeVec(db);
  }
  return 0;
}

