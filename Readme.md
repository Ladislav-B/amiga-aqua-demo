# Amiga Akvárium - Rozšířený Tutoriál Grafické Animace a AI

Vítejte v análech projektu **Amiga Akvárium**! Tento projekt je pokročilou ukázkou programování herní grafiky a umělé inteligence pro legendární platformu Amiga (OCS/ECS/AGA). Nyní obsahuje nejen plynulou animaci, ale i komplexní interakce mezi objekty.

## Architektura a Magie Projektu

Náš podmořský svět je rozdělen do několika rytířských svitků, z nichž každý má svou nezastupitelnou roli:

### 1. Srdce Programu (`main.c`)
Zde bije srdce celého akvária. Hlavní smyčka synchronizuje všechny součásti, zajišťuje double buffering pomocí `ScrollVPort` a stará se o správné pořadí vykreslování (obnova pozadí -> pohyb objektů -> Gels vykreslení -> clipping).

### 2. Správa Ryb a Zrcadlení (`fish.c`, `fish.h`)
Tato část se stará o naše ryby. 
*   **Dynamické Otáčení:** Aby ryby necouvaly, systém podle směru pohybu (`dx`) přepíná grafiku mezi `g77` (doleva) a `g88` (doprava).
*   **Stabilita Gels:** Protože systém Bobů je citlivý na změny ukazatelů, používáme metodu `CopyMem` pro aktualizaci masky (`ImageShadow`) v existujícím bufferu.

### 3. Inteligentní Hejno (`flocking.c`, `flocking.h`)
Implementace algoritmu **Boids**. Ryby se řídí pravidly separace, soudržnosti a zarovnání. 
*   **Faktor Strachu:** Ryby vnímají pozici žraloka. Pokud se přiblíží, aktivuje se čtvrté pravidlo - **útěk (fleeing)**, kdy ryby zmateně prchají od predátora.
*   **Celočíselná Matematika:** Všechny výpočty jsou prováděny v 32-bitových celých číslech (`LONG`), což zajišťuje rychlost i prevenci přetečení (overflow).

### 4. Predátor Žralok (`zralok.c`, `zralok.h`)
Náš nový majestátní obyvatel o rozměrech 64x32 pixelů. 
*   **Generování Masky:** Protože data žraloka nemají vlastní masku, program ji při startu automaticky vygeneruje (pomocí operace OR přes všechny tři barevné roviny). To zajišťuje jeho správnou průhlednost nad pozadím.

### 5. Barevná Hloubka (`more_copper.c`, `more_copper.h`)
Využívá koprocesor **Copper** k vytvoření plynulého osmibarevného modrého gradientu v pozadí. Toto kouzlo nezabírá žádný čas procesoru.

### 6. Náhodná Čísla a Nástroje (`random.c`, `animtools/`)
Zajišťují přirozenou variabilitu pohybu a poskytují nízkoúrovňové funkce pro práci se systémem Gels (Boby).

## Datové Svitky (Grafika)
*   `ryba2_sprite.h` (`g77`): Ryba koukající doleva.
*   `ryba3_sprite.h` (`g88`): Ryba koukající doprava.
*   `zralok_levy.h`, `zralok_pravy.h`: Majestátní grafika našeho predátora.
*   `bubble_sprite.h`: Data pro bubliny stoupající k hladině.
*   `background.h`: Statický obraz pozadí akvária.

## Sestavení a Běh
K vyvolání tohoto kouzla použijte kompilátor `vbcc`:

```bash
vc +aos68k .\main.c more_copper.c flocking.c fish.c zralok.c animtools/animtools.c random.c -o a.exe -lamiga
```

## Rytířova Poselství
Nejdůležitějším poznatkem z tohoto questu bylo, že interakce mezi hejnem a predátorem vytváří dynamický a stále se měnící vizuální zážitek. Vždy pamatujte na stabilitu paměti Amigy a k hardware přistupujte s úctou a precizností!

Ať vaše ryby plují vesele a žralok je vždy sytý (ale ne našimi rybkami)!

*Sepsáno s rytířskou ctí a úsměvem,*
**Ser Amík z Comodorie**
