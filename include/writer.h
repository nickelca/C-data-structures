#ifndef WRITER_H
#define WRITER_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

// clang-format off

struct Writer {
    void *ctx;
    ssize_t (*write_fn)(void *ctx, const char *buf, size_t count);
};

struct Writer
Get_STD_Out(void);

ssize_t
Write(struct Writer writer, const char *buf, size_t count);

int
Write_All(struct Writer writer, const char *buf, size_t count);

/// TODO: Implement
int
Print(struct Writer writer, const char *format, ...);

int
Write_Byte(struct Writer writer, char byte);

int
Write_Byte_N_Times(struct Writer writer, char byte, size_t n);

int
Write_Bytes_N_Times(struct Writer writer, const char *bytes, size_t count, size_t n);

#endif
