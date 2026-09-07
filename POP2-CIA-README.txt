페르시아의 왕자 2 - DOSBox 3DS 전용 CIA 소스

Behavior
- CIA contains the supplied POP2 DOS game in RomFS.
- First launch copies missing bundled files to sdmc:/3ds/POP2/.
- Existing files are never overwritten, preserving game config/high scores/saves.
- DOSBox configuration picker and splash are skipped.
- C: is mounted directly to sdmc:/3ds/POP2/.
- PRINCE.EXE runs automatically; DOSBox exits when the game returns.

Home Menu metadata
- Title: 페르시아의 왕자 2
- Product code: CTR-P-POP2
- Unique ID: 0xD5042

Icon/banner
- ctr/icon.png and ctr/banner.png have been replaced with the user-provided Prince of Persia 2 image.
- Replace these two PNGs with the desired game artwork before the final build.

Build requirement
- devkitPro 3ds-dev toolchain and 3DS portlibs compatible with this DOSBox-3DS source.
- Run: make -f Makefile.ctr
