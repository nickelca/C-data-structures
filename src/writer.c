#include "../include/writer.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <wchar.h>

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

int
Write_All_String(struct Writer writer, const char *str)
{
    return Write_All(writer, str, strlen(str));
}

int
Write_All_String_N(struct Writer writer, const char *str, size_t n)
{
    const char *len = memchr(str, 0, n);
    return Write_All(writer, str, len ? len - str : n);
}

struct Format_Placeholder {
    enum : char {
        format_length_default,
        format_length_short,
        format_length_long,
        format_length_long_long,
        format_length_ushort,
        format_length_uint,
        format_length_ulong,
        format_length_ulong_long,
    } length;
    enum : char {
        format_specifier_default,
        format_specifier_decimal,
        format_specifier_hex,
        format_specifier_octal,
        format_specifier_char,
        format_specifier_string,
    } specifier;
    char fill;
    enum : char {
        format_alignment_left,
        format_alignment_middle,
        format_alignment_right,
    } alignment;
    int width;
    int prec;
};

/// TODO: Support wchar_t?
static inline int
Format_String(struct Writer writer, struct Format_Placeholder fmt, va_list args)
{
    int ret;
    if (fmt.length == format_length_default) {
        const char *str = va_arg(args, char *);
        size_t len = 0;
        if (fmt.width < 0) {
            len = strlen(str);
        } else {
            const char *end = memchr(str, 0, fmt.width);
            len = end ? end - str : fmt.width;
        }

        ret = Write_All(writer, str, len);
        if (ret != 0) {
            return ret;
        }
    } else {
        return E_Writer_Format_Length;
    }
    return 0;
}

/// Do not include { and }
/// *out is indeterminate upon error
static int
Parse_Format_Placeholder(const char *str, size_t len, struct Format_Placeholder *out)
{
    *out = (struct Format_Placeholder){
        .length = format_length_default,
        .specifier = format_specifier_default,
        .fill = ' ',
        .alignment = format_alignment_left,
        .width = -1,
        .prec = -1,
    };

    const char *sep_opt = memchr(str, ':', len);
    const size_t sep = sep_opt
        ? (size_t)(sep_opt - str)
        : len;
    if (sep == 0) {
        return E_Writer_Format_Specifier;
    }
    const size_t length_modifier_len = sep - 1;

    switch (str[length_modifier_len]) {
        case 'd':
            out->specifier = format_specifier_decimal;
            break;
        case 'x':
            out->specifier = format_specifier_hex;
            break;
        case 'o':
            out->specifier = format_specifier_octal;
            break;
        case 'c':
            out->specifier = format_specifier_char;
            break;
        case 's':
            out->specifier = format_specifier_string;
            break;
        default: return E_Writer_Format_Specifier;
    }

    if (length_modifier_len == 0) {
        out->length = format_length_default;
    } else if (strncmp("s", str, length_modifier_len) == 0) {
        out->length = format_length_short;
    } else if (strncmp("l", str, length_modifier_len) == 0) {
        out->length = format_length_short;
    } else if (strncmp("ll", str, length_modifier_len) == 0) {
        out->length = format_length_short;
    } else if (strncmp("us", str, length_modifier_len) == 0) {
        out->length = format_length_short;
    } else if (strncmp("u", str, length_modifier_len) == 0) {
        out->length = format_length_short;
    } else if (strncmp("ul", str, length_modifier_len) == 0) {
        out->length = format_length_short;
    } else if (strncmp("ull", str, length_modifier_len) == 0) {
        out->length = format_length_short;
    } else {
        return E_Writer_Format_Length;
    }

    return 0;
}

/// TODO: Implement
int
Print(struct Writer writer, const char *format, ...)
{
    int ret = 0;
    const size_t format_len = strlen(format);
    va_list args;
    va_start(args, format);
    size_t i = 0;
    while (i < format_len) {
        size_t literal_begin = i;
        while (i < format_len) {
            if (format[i] == '{' || format[i] == '}') {
                break;
            }
            i += 1;
        }
        size_t literal_end = i;
        bool unescape_brace = false;

        if (i + 1 < format_len && format[i + 1] == format[i]) {
            unescape_brace = true;
            literal_end += 1;
            i += 2;
        }
        if (literal_begin != literal_end) {
            ret = Write_All(writer, format + literal_begin, literal_end - literal_begin);
            if (ret != 0) {
                goto ret;
            }
        }

        if (unescape_brace) {
            continue;
        }
        if (i >= format_len) {
            break;
        }

        if (format[i] == '}') {
            ret = E_Writer_Format_Start;
            goto ret;
        }
        // skip {
        i += 1;
        size_t fmt_begin = i;
        while (i < format_len && format[i] != '}') {
            i += 1;
        }
        size_t fmt_end = i;
        if (i >= format_len) {
            ret = E_Writer_Format_End;
            goto ret;
        }
        // skip }
        i += 1;

        struct Format_Placeholder placeholder;
        ret = Parse_Format_Placeholder(
            format + fmt_begin,
            fmt_end - fmt_begin,
            &placeholder
        );
        if (ret != 0) {
            goto ret;
        }

        switch (placeholder.specifier) {
            case format_specifier_string:
                Format_String(writer, placeholder, args);
                break;
            default: unreachable();
        }

    }
ret:
    va_end(args);
    return ret;
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
