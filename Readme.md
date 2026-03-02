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
*   Jakmile je vykreslování hotové, prohodíme je pomocí `db->screen->ViewPort.RasInfo->BitMap` a zavoláme `MakeScreen` a `RethinkDisplay`.
*   Tímto způsobem se divák vždy dívá na hotový obrázek, zatímco my "za oponou" připravujeme další.

### 3. Čisté Pozadí a Clipping (Background Restoration)

Zde přichází trik starých mistrů! Máme **třetí bitmapu** (`bgBitMap`), která slouží jako "čistý zdroj" pozadí. Do této bitmapy se **nikdy nekreslí** žádné ryby ani bubliny. Obsahuje pouze statické pozadí akvária.

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
*   **Výhoda:** Systém Gels se stará o ukládání a obnovování pozadí pod sprajty (v našem případě to děláme ručně pro maximální rychlost s `bgBitMap`, ale Gels nám stále řeší vykreslování masky a obrazu).

### 5. Herní Smyčka (Game Loop)

Srdce programu bije ve smyčce `while`.

1.  **Synchronizace:** Čekáme na vertikální paprsek (`WaitTOF`), abychom se synchronizovali s obnovovací frekvencí monitoru (50Hz PAL / 60Hz NTSC).
2.  **Logika:** Aktualizujeme souřadnice `x, y` ryb a bublin.
    *   Ryby plavou horizontálně.
    *   Bubliny stoupají vzhůru s náhodným kmitáním (`randRange`).
    *   Když objekt opustí obrazovku, resetujeme jeho pozici na opačnou stranu.
3.  **Vykreslení:** Voláme `DrawGList` pro vykreslení nové polohy do *Back Bufferu*.
4.  **Prohození:** Prohodíme buffery a cyklus se opakuje.

## Datové Struktury

Pro lepší organizaci kódu používáme vlastní struktury:

```c
struct Fish {
    struct Bob *bob; // Odkaz na grafický objekt
    WORD x, y;       // Pozice
    WORD dx;         // Rychlost a směr (kladné = doprava, záporné = doleva)
    WORD width;      // Šířka ryby pro správné ošetření okrajů
};
```

## Odkazy na Studnici Moudrosti (Zdroje)

Všechny znalosti pro tento projekt byly čerpány z legendárních svitků Amiga ROM Kernel Reference Manuals.

*   [Amiga Developer Docs (Elowar)](http://amigadev.elowar.com/) - Hlavní brána k vědění.
*   [Graphics Library](http://amigadev.elowar.com/read/ADCD_2.1/Includes_and_Autodocs_2._guide/node02E1.html) - Vše o `RastPort`, `BitMap`, `BltBitMap`.
*   [Intuition Library](http://amigadev.elowar.com/read/ADCD_2.1/Includes_and_Autodocs_3._guide/node0278.html) - Práce s obrazovkami (`OpenScreen`).

## Rytířovy Postřehy a Budoucí Vylepšení

Jako Ser Amík z Comodorie vidím v tomto projektu velký potenciál, ale pravý rytíř se nikdy nepřestává zdokonalovat! Zde je pár nápadů na další questy:

1.  **Vylepšení Časování:** Nyní je rychlost závislá na `WaitTOF` (50 FPS). Bylo by šlechetné implementovat delta-time časování, aby rybky plavaly stejně rychle na A500 i na A4000.
2.  **Inteligentnější Ryby:** Přidat hejnové chování (flocking) nebo reakci na myš (utíkání před kurzorem).
3.  **Měděná Magie (Copper Lists):** Použít Copper list pro vytvoření gradientního pozadí (vody), které se mění s hloubkou, bez zátěže CPU.
4.  **Zvukové Efekty:** Přidat `audio.device` pro bublání a šplouchání.

Ať vás provází síla Guru Meditation (ale jen v dobrém)!

*Sepsáno s úctou,*
**Ser Amík z Comodorie**
