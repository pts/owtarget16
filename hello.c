/*
 * hello.c: hello-world program for ELKS 8086 in OpenWatcom C, temporarily targeting OS/2
 * by pts@fazekas.hu at Thu Jan 19 20:26:29 CET 2023
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

#define MSG "Hello, World!\n"
static const char msg[sizeof(MSG) - 1] = MSG;

void __cdecl start_() {
  write_inline(1, msg, sizeof(msg));
  exit_inline(0);
}
