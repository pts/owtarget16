/*
 * rot13.c: rot13 converter for ELKS 8086 in OpenWatcom C, temporarily targeting OS/2
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
typedef unsigned mode_t;

#define NULL ((void *)0)

#define SEEK_SET 0  /* whence value below. */
#define SEEK_CUR 1
#define SEEK_END 2

#define O_RDONLY 0  /* flags bitfield value below. */
#define O_WRONLY 1
#define O_RDWR   2

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#if 1
#define SYSCALL_ASM "int 80h"  "test ax, ax"  "jns short done"  "or ax, -1"  "done:"  /* 9 bytes, with jump. */
#else
#define SYSCALL_ASM "int 80h"  "cmp ax, 8000h"  "cmc"  "sbb bx, bx"  "or ax, bx"  /* 12 bytes, without a jump. Doesn't help OpenWatcom with tail optimizations. */
#endif

/* TODO(pts): Optimize syscalls by size. */
static ssize_t read_inline(int fd, void *buf, size_t count);
#pragma aux read_inline = "mov ax, 3"  SYSCALL_ASM  value [ ax ] parm [ bx ] [ cx ] [ dx ] modify [ ax ];

static ssize_t write_inline(int fd, const void *buf, size_t count);
#pragma aux write_inline = "mov ax, 4"  SYSCALL_ASM  value [ ax ] parm [ bx ] [ cx ] [ dx ] modify [ ax ];

/* TODO(pts): open2(...), open3(...), `...'. */
static int open_inline(const char *pathname, int flags, mode_t mode);
#pragma aux open_inline = "mov ax, 5"  SYSCALL_ASM  value [ ax ] parm [ bx ] [ cx ] [ dx ] modify [ ax ];

__declspec(aborts) static void exit_inline(int status);
#pragma aux exit_inline = "xchg ax, bx"  "xor ax, ax"  "inc ax"  "int 80h"  parm [ ax ];

static char buf[0x200];

#define ERRMSG_OPEN "fatal: error opening rot13 input\n"
static const char errmsg_open[sizeof(ERRMSG_OPEN) - 1] = ERRMSG_OPEN;

#define ERRMSG_READ "fatal: error reading\n"
static const char errmsg_read[sizeof(ERRMSG_READ) - 1] = ERRMSG_READ;

#define ERRMSG_WRITE "fatal: error writing\n"
static const char errmsg_write[sizeof(ERRMSG_WRITE) - 1] = ERRMSG_WRITE;

/* Must not be called `main', otherwise the OpenWatcom C compiler pulls in
 * the OpenWatcom libc.
 */
int prog_main(int argc, char **argv) {
  int fd = STDIN_FILENO;
  int got, remaining;
  unsigned u;
  char *p;
  (void)argc;
  if (argv[0] && argv[1] && !(argv[1][0] == '-' && argv[1][1] == '\0')) {
    if ((fd = open_inline(argv[1], O_RDONLY, 0)) < 0) {
      (void)!write_inline(STDERR_FILENO, errmsg_open, sizeof(errmsg_open));
      return 1;
    }
  }
  while ((got = read_inline(fd, buf, sizeof(buf))) > 0) {
    for (p = buf, remaining = got; remaining > 0; ++p, --remaining) {
      /* Apply rot13 transformation to single byte *p (can be signed or unsigned char). */
      if ((u = (*p | 32) + (0U - 'a')) < 13U) {
        *p += 13;
      } else if (u - 13U < 13U) {
        *p -= 13;
      }
    }
    for (p = buf, remaining = got; remaining > 0; p += got, remaining -= got) {
      if ((got = write_inline(STDOUT_FILENO, p, remaining)) <= 0) {
        (void)!write_inline(STDERR_FILENO, errmsg_write, sizeof(errmsg_write));
        return 1;
      }
    }
  }
  if (got < 0) {
    (void)!write_inline(STDERR_FILENO, errmsg_read, sizeof(errmsg_read));
    return 1;
  }
  return 0;
}

__declspec(aborts) static void start_inline(void);
#pragma aux start_inline = "pop ax"  "mov dx, sp"  "call prog_main"  "xchg ax, bx"  "xor ax, ax"  "inc ax"  "int 80h";
__declspec(aborts) void _start(void) { start_inline(); }
