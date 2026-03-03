#include <proto/graphics.h>
#include <proto/exec.h>
#include <graphics/gfxmacros.h>
#include <hardware/custom.h>
#include "more_copper.h"

/* 
 * Deklarace custom pro makra CMOVE. 
 */
extern volatile struct Custom custom;

struct UCopList* create_gradient(void) {
    struct UCopList *uCopList = NULL;

    /* Alokace hlavičky v PUBLIC paměti */
    uCopList = (struct UCopList *)AllocVec(sizeof(struct UCopList), MEMF_PUBLIC | MEMF_CLEAR);
    if (!uCopList) return NULL;

    /* Inicializace pro 30 instrukcí (8 kroků CWAIT + CMOVE bohatě stačí) */
    CINIT(uCopList, 30);

    /* 
     * Hlubokomořský gradient - 8 kroků.
     * Měníme barvu 0 (pozadí) od hladiny k hlubině.
     */
    
    CWAIT(uCopList, 20, 0);
    CMOVE(uCopList, custom.color[0], 0x0223); /* Horní vrstva - mořská modř */

    CWAIT(uCopList, 50, 0);
    CMOVE(uCopList, custom.color[0], 0x0113);

    CWAIT(uCopList, 85, 0);
    CMOVE(uCopList, custom.color[0], 0x0112);

    CWAIT(uCopList, 120, 0);
    CMOVE(uCopList, custom.color[0], 0x0002);

    CWAIT(uCopList, 155, 0);
    CMOVE(uCopList, custom.color[0], 0x0001);

    CWAIT(uCopList, 190, 0);
    CMOVE(uCopList, custom.color[0], 0x0001);

    CWAIT(uCopList, 225, 0);
    CMOVE(uCopList, custom.color[0], 0x0000); /* Temná hlubina */

    CWAIT(uCopList, 250, 0);
    CMOVE(uCopList, custom.color[0], 0x0000); /* Úplné dno */

    CEND(uCopList);

    return uCopList;
}
