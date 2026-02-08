# PPLSYLLABUSDS

Minimal Nintendo DS homebrew syllabus tracker for a Private Pilot License (PPL) training plan.

## Build

1. Ensure `DEVKITPRO` is set and devkitARM + libnds are installed.
2. Run:

```sh
make
```

The output `.nds` is generated at:

```
PPLSYLLABUSDS.nds
```

## Install (for my daughter)

1. Copy `PPLSYLLABUSDS.nds` to your flashcart SD card.
2. Launch it from the flashcart menu or TWiLight Menu++.
3. A save file will appear automatically if the flashcart supports FAT saves.

## Controls

**Global**
- `START`: Quick jump to last day from the main menu.
- `SELECT`: Save now.

**Menu**
- D-Pad: Move cursor
- `A`: Select
- Touch: Tap a menu button

**Day Browser**
- `L/R`: Previous/next day
- `UP/DOWN`: Scroll content
- `A`: Toggle completion (flight/ground/hw) or edit notes
- `B`: Back (or exit notes edit)
- Touch: Tabs, checkbox, or `[EDIT]`

**Requirements**
- `UP/DOWN`: Select item
- `LEFT/RIGHT`: Decrease/increase hours by 0.1 (for counters)
- `A`: Toggle checkbox
- `B`: Back

**Flashcards**
- `L/R`: Previous/next card
- `A`: Reveal/hide answer
- `B`: Back
