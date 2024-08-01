#! /bin/sh --
# by pts@fazekas.hu at Thu Jan 19 20:20:26 CET 2023
set -ex

# gcc -W -Wall -s -O2 -o prog16cv prog16cv.c  # Any host.
owcc -Wall -Wextra -s -O2 -fno-stack-check -o prog16cv prog16cv.c  # For the Linux host.

for PROG in hello printarg rot13; do
  # There is no -nostdinc.
  # OpenWatcom 2 C compiler. https://open-watcom.github.io/
  # -Wl,disable -Wl,1014  hides this: Warning! W1014: stack segment not found
  # !! Specify the stack size in the .c file (?).
  # Stack size: 0x400 is enough, 0x380 seems to be too low (fails with `"$PROG": Out of space) in ELKS 0.2.0.
  owcc -bos2 -march=i86 -I${WATCOM}/h -fnostdlib -Wl,disable -Wl,1014 -Wl,option -Wl,start=_start_ -Wl,option -Wl,dosseg -Wl,option -Wl,stack=0x400 -Os -s -fno-stack-check -Wall -Wextra -o "$PROG".os2 "$PROG".c
  ./prog16cv -f elks -o "$PROG".elks "$PROG".os2
  rm -f "$PROG".os2
done

: "$0" OK.
