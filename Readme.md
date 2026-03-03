# Amiga Akvárium - Tutoriál Grafické Animace

Vítejte, udatní programátoři a rytíři kódu! Představuji vám projekt **Amiga Akvárium**, demonstraci plynulé animace a umělé inteligence na platformě Amiga. V tomto projektu ožívají rybky a bubliny v podmořském světě, využívajíce sílu čipsetu OCS/ECS a operačního systému AmigaOS.

## Jak to funguje: Architektura a Magie

Celý projekt je postaven na nízkoúrovňovém přístupu k hardwaru Amigy s využitím systémových knihoven `graphics.library` a `intuition.library`.

### 1. Copper: Magie Barevného Gradientu

Pro efekt hlubokého moře využíváme koprocesor **Copper**. Ten mění barvu pozadí (index 0) přímo během vykreslování řádků.
*   **Implementace:** V `more_copper.c` vytváříme `UCopList` s 8 odstíny modré.
*   **Výhoda:** Plynulý barevný přechod bez jakéhokoliv zatížení hlavního procesoru (CPU).

### 2. Inteligentní Hejno (Flocking / Boids)

Ryby se nepohybují náhodně, ale jako společenstvo řízené algoritmem **Boids** (podle Craiga Reynoldse). Každá ryba se rozhoduje na základě tří rytířských pravidel:

1.  **Separace (Separation):** Ryba se snaží udržovat bezpečný rozestup od svých sousedů, aby nedošlo ke srážce.
2.  **Soudržnost (Cohesion):** Ryba je přitahována ke středu hejna, aby družina zůstala pohromadě.
3.  **Zarovnání (Alignment):** Ryba se snaží sjednotit svůj směr a rychlost s ostatními v hejnu.

**Technické detaily implementace:**
*   **Celočíselná matematika:** Aby procesor 68000 neztrácel dech, nepoužíváme čísla s plovoucí čárkou (floats), ale čistou celočíselnou aritmetiku.
*   **Prevence přetečení (Overflow):** Výpočty vzdáleností provádíme pomocí 32-bitových čísel (`LONG`). Při použití 16-bitových čísel by při větších vzdálenostech došlo k přetečení, což by vedlo k "šílenému" chování ryb a zamrznutí systému.
*   **Odrazy:** Pokud ryba narazí na hranici akvária, je jemně odražena zpět do středu dění.

### 3. Dynamické Otáčení a Správa Bobů

Naše ryby mají různé grafické předlohy (`g77` kouká doleva, `g88` doprava). Aby ryby při plavbě necouvaly, implementovali jsme systém dynamického přepínání grafiky v `fish.c`.

*   **Prohazování obrazu:** Podle směru pohybu (`dx`) měníme ukazatel `ImageData` ve struktuře `VSprite`.
*   **Kopírování masky:** Protože systém Gels (Boby) je velmi citlivý na změny ukazatelů masek, nepřenastavujeme ukazatel na masku, ale pomocí `CopyMem` kopírujeme nová data masky do stávajícího bufferu `ImageShadow`. To zaručuje 100% stabilitu bez pádů (Guru Meditation).

### 4. Grafický Systém a Double Buffering

*   **Double Buffering:** Používáme dvě bitmapy. Zatímco se jedna zobrazuje, do druhé Blitter kreslí další snímek. Prohazování provádíme pomocí `ScrollVPort`.
*   **Obnova Pozadí:** Máme třetí, čistou bitmapu (`bgBitMap`). V každém snímku z ní pomocí Blitteru zkopírujeme čisté pozadí do pracovní bitmapy, čímž "smažeme" ryby z předchozího snímku.
*   **Gels & BOBs:** Ryby a bubliny jsou vykreslovány jako **Blitter Objects (BOBs)**, což umožňuje hardwarové maskování a plynulé překrývání.

## Struktura Projektu

*   `main.c`: Srdce programu a hlavní smyčka.
*   `fish.c/h`: Správa ryb, jejich inicializace a otáčení grafiky.
*   `flocking.c/h`: Inteligence hejna a matematické výpočty pohybu.
*   `more_copper.c/h`: Kouzlo hlubokomořského gradientu.
*   `animtools/`: Systémové nástroje pro práci s Gels.

## Sestavení

K sestavení tohoto velkolepého díla použijte kompilátor `vbcc`:

```bash
vc +aos68k .\main.c more_copper.c flocking.c fish.c animtools/animtools.c random.c -o a.exe -lamiga
```

## Rytířovy Postřehy

Tento projekt ukazuje, že i na klasickém hardware Amigy lze s trochou matematiky a rytířské trpělivosti vytvořit komplexní a živý svět. Nejdůležitějším ponaučením z tohoto questu bylo: **"Vždy si hlídej své 32-bitové registry při násobení souřadnic!"**

Ať vaše ryby plují vesele a vaše Amiga nikdy nezažije Guru Meditation!

*Sepsáno s úctou a úsměvem,*
**Ser Amík z Comodorie**
