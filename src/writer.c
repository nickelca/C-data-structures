#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "../include/writer.h"

// clang-format off

static size_t
STD_Out_Write(void *ctx, const char *buf, size_t count)
{
    return fwrite(buf, 1, count, ctx);
}

struct Writer
Get_STD_Out(void)
{
    static struct Writer stdout_w = {};
    stdout_w.ctx = stdout;
    stdout_w.write_fn = STD_Out_Write;
    return stdout_w;
}

size_t
Write(struct Writer writer, const char *buf, size_t count)
{
    return writer.write_fn(writer.ctx, buf, count);
}

int
Write_All(struct Writer writer, const char *buf, size_t count)
{
    for (size_t i = 0; i < count;) {
        size_t written = Write(writer, buf, count);
        if (written < count) {
            return E_Writer_Short_Count;
        }
        i += written;
    }
    return 0;
}

/// TODO: Implement
int
Print(struct Writer writer, const char *format, ...)
{
    int ret;
    const size_t format_len = strlen(format);
    size_t i = 0;
    while (i < format_len) {
        size_t start_i = i;
        while (i < format_len) {
            if (format[i] == '{' || format[i] == '}') {
                break;
            }
            i += 1;
        }
        size_t end_i = i;
        bool unescape_brace = false;

        if (i + 1 < format_len && format[i + 1] == format[i]) {
            unescape_brace = true;
            end_i += 1;
            i += 2;
        }

        if (start_i != end_i) {
            ret = Write_All(writer, format + start_i, end_i - start_i);
            if (ret != 0) {
                return ret;
            }
        }

        if (unescape_brace) {
            continue;
        }

        if (i >= format_len) {
            break;
        }

        if (format[i] == '}') {
            return E_Writer_Placeholder_Start;
        }
        i += 1;

        size_t fmt_begin = i;
        while (i < format_len && format[i] != '}') {
            i += 1;
        }
        size_t fmt_end = i;

        if (i >= format_len) {
            return E_Writer_Placeholder_End;
        }
        i += 1;

    }
    return 0;
}

int
Write_Byte(struct Writer writer, char byte)
{
    return Write_All(writer, &byte, 1);
}

int
Write_Byte_N_Times(struct Writer writer, char byte, size_t n)
{
    constexpr const size_t bytes_len = 256;
    char bytes[bytes_len];
    memset(bytes, byte, n);

    while (n > 0) {
        size_t to_write = (n < bytes_len) ? n : bytes_len;
        int ret = Write_All(writer, bytes, to_write);
        if (!ret) {
            return ret;
        }
        n -= to_write;
    }
    return 0;
}

int
Write_Bytes_N_Times(struct Writer writer, const char *bytes, size_t count, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        int ret = Write_All(writer, bytes, count);
        if (ret != 0) {
            return ret;
        }
    }
    return 0;
}
