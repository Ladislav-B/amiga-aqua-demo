# Amiga Akvárium - Tutoriál Grafické Animace

Vítejte, udatní programátoři a rytíři kódu! Představuji vám projekt **Amiga Akvárium**, demonstraci plynulé animace na platformě Amiga. V tomto projektu ožívají rybky a bubliny v podmořském světě, využívajíce sílu čipsetu a operačního systému AmigaOS.

Tento projekt slouží jako **tutoriál** pro ty, kteří chtějí proniknout do tajů programování grafiky v jazyce C na Amize.

## Jak to funguje: Architektura a Magie

Celý projekt je postaven na nízkoúrovňovém přístupu k hardwaru Amigy s využitím systémových knihoven `graphics.library` a `intuition.library`. Zde je rozbor klíčových mechanik.

### 1. Otevření Obrazovky (The Screen)

Naše dobrodružství začíná otevřením vlastní obrazovky (Custom Screen). Nepoužíváme standardní Workbench, ale vytváříme si vlastní píseček.

*   **Funkce:** `OpenScreenTagList` ([Amiga Dev: OpenScreenTagList](http://amigadev.elowar.com/read/ADCD_2.1/Includes_and_Autodocs_3._guide/node02E6.html))
*   **Vlastnosti:** Rozlišení 640x256 (HiRes), 4 bitplanes (16 barev).
*   **Cíl:** Získat pointer na `struct Screen`, který je naším plátnem.

### 2. Double Buffering (Dvojitá Vyrovnávací Paměť)

Aby byl pohyb rybek plynulý a bez blikání (flickering), používáme techniku **Double Buffering**.

*   Máme dvě bitmapy (`BitMap`):
    *   **Front Buffer:** To, co právě vidí uživatel na monitoru.
    *   **Back Buffer:** Skrytá bitmapa, do které kreslíme další snímek animace.
*   Jakmile je vykreslování hotové, prohodíme je pomocí `db->screen->ViewPort.RasInfo->BitMap`.
*   Tímto způsobem se divák vždy dívá na hotový obrázek, zatímco my "za oponou" připravujeme další.

### 3. Čisté Pozadí a Clipping (Background Restoration)

Zde přichází trik starých mistrů! Máme **třetí bitmapa** (`bgBitMap`), která slouží jako "čistý zdroj" pozadí. Do této bitmapy se **nikdy nekreslí** žádné ryby ani bubliny. Obsahuje pouze statické pozadí akvária.

V každém snímku (frame) děláme toto:
1.  **Vymazání stop:** Pomocí `BltBitMap` ([Amiga Dev: BltBitMap](http://amigadev.elowar.com/read/ADCD_2.1/Includes_and_Autodocs_2._guide/node0334.html)) zkopírujeme čisté pozadí z `bgBitMap` do našeho *Back Bufferu*. Tím efektivně "smažeme" vše, co tam bylo v minulém snímku.
2.  **Clipping (Ořez):** Po vykreslení ryb znovu použijeme `BltBitMap` k přepsání okrajů obrazovky čistým pozadím. Tím zajistíme, že rybky "zmizí", když vplují mimo vymezenou oblast, aniž bychom museli složitě počítat masky na okrajích.

### 4. Gels Systém a BOBs (Blitter Objects)

Pro naše ryby a bubliny používáme systém **Gels** (Graphics Elements), konkrétně **BOBs** (Blitter Objects).

*   **Struktura:** Každá ryba je reprezentována strukturou `struct Bob`.
*   **Funkce:**
    *   `makeBob`: Vytvoří BOBa z definice.
    *   `AddBob`: Přidá BOBa do seznamu k vykreslení pro daný `RastPort`.
    *   `DrawGList`: Vykreslí všechny aktivní objekty do bufferu pomocí Blitteru ([Amiga Dev: DrawGList](http://amigadev.elowar.com/read/ADCD_2.1/Includes_and_Autodocs_2._guide/node036D.html)).

### 5. Copper: Magie Barevného Gradientu

Pro dosažení efektu hlubokého moře využíváme sílu koprocesoru **Copper**. Ten dokáže měnit barvy palety přímo během vykreslování řádků na monitoru.

*   **Mechanika:** Vytváříme uživatelský Copper list (`UCopList`), který systém automaticky zařadí do hlavního instrukčního toku hardware.
*   **Implementace:** V souboru `more_copper.c` definujeme 8 odstínů modré barvy pro barevný index 0 (pozadí).
*   **Funkce:** Používáme instrukce `CWAIT` (počkej na řádek) a `CMOVE` (změň barvu v registru `color[0]`).
*   **Výsledek:** Plynulý barevný přechod od hladiny až ke dnu, aniž by to zatěžovalo hlavní procesor (CPU).

### 6. Herní Smyčka (Game Loop)

Srdce programu bije ve smyčce `while`.

1.  **Synchronizace:** Čekáme na vertikální paprsek (`WaitTOF`), abychom se synchronizovali s obnovovací frekvencí monitoru (50Hz PAL / 60Hz NTSC).
2.  **Logika:** Aktualizujeme souřadnice `x, y` ryb a bublin.
3.  **Vykreslení:** Voláme `DrawGList` pro vykreslení nové polohy do *Back Bufferu*.
4.  **Prohození:** Prohodíme buffery pomocí `ScrollVPort` nebo `MakeScreen/RethinkDisplay` pro aktivaci Copperu a zobrazení nové bitmapy.

## Datové Struktury

Pro lepší organizaci kódu používáme vlastní struktury:

```c
struct Fish {
    struct Bob *bob; // Odkaz na grafický objekt
    WORD x, y;       // Pozice
    WORD dx;         // Rychlost a směr
    WORD width;      // Šířka ryby
};
```

## Odkazy na Studnici Moudrosti (Zdroje)

Všechny znalosti pro tento projekt byly čerpány z legendárních svitků Amiga ROM Kernel Reference Manuals.

*   [Amiga Developer Docs (Elowar)](http://amigadev.elowar.com/) - Hlavní brána k vědění.
*   [Graphics Library](http://amigadev.elowar.com/read/ADCD_2.1/Includes_and_Autodocs_2._guide/node02E1.html) - Vše o `RastPort`, `BitMap`, `BltBitMap` a `UCopList`.
*   [Intuition Library](http://amigadev.elowar.com/read/ADCD_2.1/Includes_and_Autodocs_3._guide/node0278.html) - Práce s obrazovkami (`OpenScreen`).

## Sestavení

K sestavení projektu použijte kompilátor `vbcc` s následujícím příkazem:

```bash
vc +aos68k .\main.c animtools/animtools.c random.c more_copper.c flocking.c -o a.exe -lamiga
```

## Rytířovy Postřehy a Budoucí Vylepšení

Jako Ser Amík z Comodorie vidím v tomto projektu velký potenciál! Zde je pár nápadů na další questy:

1.  **Vylepšení Časování:** Implementovat delta-time časování pro konzistentní rychlost na všech procesorech.
2.  **Inteligentnější Ryby:** Přidat hejnové chování (flocking) nebo reakci na myš.
3.  **Dynamický Copper:** Animovat barvy v Copper listu pro efekt tekoucí vody nebo záblesků světla.

Ať vás provází síla Guru Meditation (ale jen v dobrém)!

*Sepsáno s úctou a úsměvem,*
**Ser Amík z Comodorie**
