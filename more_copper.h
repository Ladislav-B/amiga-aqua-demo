#ifndef MORE_COPPER_H
#define MORE_COPPER_H

#include <graphics/gfxnodes.h>
#include <graphics/view.h>

/* 
 * Funkce vytvoří uživatelský Copper list pro gradient hlubokého moře.
 * Vrací ukazatel na UCopList nebo NULL při nezdaru.
 */
struct UCopList* create_gradient(void);

#endif
