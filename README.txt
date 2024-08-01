owtarget16: targeting ELKS and other 8086 operating systems with the
OpenWatcom C compiler
^^^^^^^^^^^^^^^^^^^^^
owtarget16 is a set of tools and libraries for writing C programs with the
OpenWatcom C compiler, targeting ELKS and other operating systems for the
Intel 8086 (16-bit x86). owtarget16 contains a cross-compiler: code for the
16-bit 8086 target can be compiled on Linux or other (typically 32-bit or
64-bit) operating systems for which the OpenWatcom C compiler is available.

OpenWatcom itself supports the following 16-bit x86 targets: DOS 16-bit
(.com progrms, .exe programs with various memory models), OS/2 1.x 16-bit
and Microsoft Windows 3.x 16-bit. However, there were and are a few other
16-bit x86 operating systems (e.g. ELKS, Minix, Coherent, Xenix, CP/M-86)
for which OpenWatcom lacks libc and file format support. This project,
owtarget16 adds these missing pieces, making it possible to target some of
those operating systems as well by writing C code and compiling it with the
OpenWatcom C compiler.

The missing pieces added by owtarget16 are:

* A libc for the target, containing the usual functions (e.g. strlen(3) and
  write(2)).
* A build script (compile.sh) which runs the OpenWatcom C compiler (owcc
  frontend) with the correct flags. Currently it uses `-bos2', so that
  OpenWatcom creates an OS/2 1.x program (as a temporary output). The
  build script also calls the program conversion tool (see below).
* A program conversion tool (prog16cv.c) to convert the temporary OS/2 1.x
  program to the target program file format (e.g. ELKS program).

Currently owtarget16 is in an experimental, proof-of-concept state. This
means that targeting ELKS works end-to-end (compilation of the example
programs and running them on ELKS withing QEMU) on Linux x86 (i386 and
amd64) hosts, and the libc contains less than 10 functions.

How to target ELKS using owtarget16:

* On a Linux i386 or amd64 system download and extract the OpenWatcom C
  compiler (https://github.com/open-watcom/open-watcom-v2/releases/download/Current-build/open-watcom-2_0-f77-linux-x86).
* Download fd1440-fat.img https://github.com/jbruchon/elks/releases/tag/v0.6.0
* In a terminal window:
  * Run: git clone https://github.com/pts/owtarget16
  * cd into the extracted directory containing binl/owcc.
  * Run: export WATCOM="$PWD"; export PATH="$WATCOM/binl:$PATH" INCLUDE="$WATCOM/lh"
  * cd to the owtarget16 directory (created by `git clone' above).
  * Run: ./compile.sh
* To try the example programs, in the same terminal window:
  * All this was tested on Ubuntu 18.04 amd64 (64-bit x86).
  * Install mtools: sudo apt-get install mtools
  * Install QEMU: sudo apt-get install qemu-system-x86
  * Move the downloaded fd1440-fat.img to ./elks060.img
  * Copy over the compiled example programs:
    * Run: chmod 755 *.elks
    * Run: mcopy -m -o -i elks060.img hello.elks ::bin/hello
    * Run: mcopy -m -o -i elks060.img printarg.elks ::bin/printarg
    * Run: mcopy -m -o -i elks060.img rot13.elks ::bin/rot13
  * Run ELKS in QEMU: qemu-system-i386 -L pc-1.0 -m 1 -net none  -drive file=elks020.img,format=raw,if=floppy
  * In the (black) ELKS QEMU window, type root and press <Enter> to log in.
  * In the ELKS QEMU window at the command prompt, run: printarg hi there
  * Close the ELKS QEMU window.

What is already implemented:

* Compiling on Linux i386 or amd64 host.
* Targeting ELKS 8086. The example programs work on ELKS 0.2.0 and 0.6.0.
* Using the OpenWatcom C compiler (owcc frontend) to compile C code.
* Experimental build script (compile.sh).
* The program conversion tool reading an OS/2 1.x program and writing an
  ELKS program.
* libc functions open(2), read(2), write(2), exit(2), strlen(3).
* _start function to initialize `environ', call main(...) and exit.

What can be implemented in the future using this design:

* Targeting more 16-bit x86 operating systems:
  * DOS 16-bit .com programs (OpenWatcom already does it)
  * DOS 16-bit .exe programs (OpenWatcom already does it)
  * OS/2 1.x 16-bit .exe programs (OpenWatcom already does it)
  * ELKS 8086 16-bit programs (already implemented)
  * Minix 1.x and 2.x 8086 16-bit programs
  * Coherent 3.x 8086 16-bit programs
  * Xenix 16-bit programs
  * CP/M-86 16-bit programs
* Creating a libc in a separate .lib file.
* Size-optimizing the existing libc functions.
* Adding (many) more functions to the libc.
* Size-optimizing the libc functions, adding alternative, smaller .lib files
  (e.g. those without stdio, those without floating point support in
  printf(3)).
* Please note that it's unlikely that GUI code will be added to the libc, so
  only text mode (terminal window, command-prompt) programs will be
  supported.
* Using any host where the OpenWatcom C compiler is available (e.g. 32-bit
  DOS and Win32 in addition to Linux). Using Docker or virtualization on
  macOS host.
* Running the OpenWatcom C compiler (owcc) and the prog16cv tool in
  qemu-i386 on non-x86 Linux host.
* Adding support for other C compilers targeting the 8086, most notably
  gcc-ia16. Probably Borland C, Microsoft C and Zortech C++ can also be
  added if there is need.

__END__
