/*
 * printargc.c: print argv and environ for ELKS 8086 in OpenWatcom C, temporarily targeting OS/2
 * by pts@fazekas.hu at Fri Jan 20 01:36:10 CET 2023
 */

#ifndef __WATCOMC__
#  error Watcom C compiler required.
#endif

#ifdef __cplusplus
#  error C (rather than C++) compiler required.
#endif

#ifndef __OS2__
#  error OS/2 target required.
#endif

#ifndef _M_I86
#  error 16-bit Intel target required.
#endif

/* ELKS 8086 target. */

typedef int ssize_t;
typedef unsigned size_t;
typedef long off_t;

#define NULL ((void *)0)

#if 1
#define SYSCALL_ASM "int 80h"  "test ax, ax"  "jns short done"  "or ax, -1"  "done:"  /* 9 bytes, with jump. */
#else
#define SYSCALL_ASM "int 80h"  "cmp ax, 8000h"  "cmc"  "sbb bx, bx"  "or ax, bx"  /* 12 bytes, without a jump. Doesn't help OpenWatcom with tail optimizations. */
#endif

static ssize_t write_inline(int fd, const void *buf, size_t count);
#pragma aux write_inline = "mov ax, 4"  SYSCALL_ASM  value [ ax ] parm [ bx ] [ cx ] [ dx ] modify [ ax ];

__declspec(aborts) static void exit_inline(int status);
#pragma aux exit_inline = "xchg ax, bx"  "xor ax, ax"  "inc ax"  "int 80h"  parm [ ax ];

static size_t strlen_inline(const char *s);  /* Unused. Maybe shorter for inlining. */
#pragma aux strlen_inline = "xor ax, ax"  "dec ax"  "again: cmp byte ptr [si], 1"  "inc si"  "inc ax"  "jnc short again"  value [ ax ] parm [ si ] modify [ si ];

extern char **environ;

/* Must not be called `main', otherwise the OpenWatcom C compiler pulls in
 * the OpenWatcom libc.
 */
int prog_main(int argc, char **argv) {
  (void)argv;
  for (; argc > 0; --argc) {
    (void)!write_inline(1, "*", 1);
  }
  (void)!write_inline(1, "\n", 1);
  for (; *argv; ++argv) {
    (void)!write_inline(1, *argv, strlen_inline(*argv));
    (void)!write_inline(1, "\n", 1);
  }
  (void)!write_inline(1, "---\n", 4);
  for (argv = environ; *argv; ++argv) {
    (void)!write_inline(1, *argv, strlen_inline(*argv));
    (void)!write_inline(1, "\n", 1);
  }
  (void)!write_inline(1, "---\n", 4);
  return 0;
}

char **environ;

/* TODO(pts): Optimize start by size, don't emit bx--environ if environ is not used. */
__declspec(aborts) static void start_inline(void);
#pragma aux start_inline = "pop ax"  "mov dx, sp"  "mov bx, sp"  "next_envvar: cmp word ptr [bx], 1"  "inc bx"  "inc bx"  "jnc next_envvar"  "mov word ptr environ, bx"  "call prog_main"  "xchg ax, bx"  "xor ax, ax"  "inc ax"  "int 80h";
__declspec(aborts) void _start(void) { start_inline(); }
