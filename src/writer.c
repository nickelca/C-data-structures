#include "../include/writer.h"
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    return Write_All(writer, str, len ? (size_t)(len - str) : n);
}

struct Format_Placeholder {
    enum : char {
        format_length_default,
        format_length_long,
        format_length_long_long,
        format_length_unsigned,
        format_length_ulong,
        format_length_ulong_long,
        format_length_usize,
    } length;
    enum : char {
        format_specifier_default,
        format_specifier_binary,
        format_specifier_decimal,
        format_specifier_octal,
        format_specifier_hex,
        format_specifier_Hex,
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
Format_String(struct Writer writer, struct Format_Placeholder fmt, va_list *args)
{
    int ret;
    if (fmt.length == format_length_default) {
        const char *str = va_arg(*args, char *);
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

static inline int
Format_Integer(struct Writer writer, struct Format_Placeholder fmt, va_list *args)
{
    int ret = 0;
    unsigned base;
    const char *digits1;
    const char *digits2;
    switch (fmt.specifier) {
    case format_specifier_binary:
        base = 2;
        digits1 = "01";
        digits2 = "00011011";
        break;
    case format_specifier_octal:
        base = 8;
        digits1 = "01234567";
        digits2 = "00010203040506071011121314151617202122232425262730313233343536374041424344454647505152535455565760616263646566677071727374757677";
        break;
    case format_specifier_decimal:
        base = 10;
        digits1 = "0123456789";
        digits2 = "00010203040506070809101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354555657585960616263646566676869707172737475767778798081828384858687888990919293949596979899";
        break;
    case format_specifier_hex:
        base = 16;
        digits1 = "0123456789abcdef";
        digits2 = "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff";
        break;
    case format_specifier_Hex:
        base = 16;
        digits1 = "0123456789ABCDEF";
        digits2 = "000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F404142434445464748494A4B4C4D4E4F505152535455565758595A5B5C5D5E5F606162636465666768696A6B6C6D6E6F707172737475767778797A7B7C7D7E7F808182838485868788898A8B8C8D8E8F909192939495969798999A9B9C9D9E9FA0A1A2A3A4A5A6A7A8A9AAABACADAEAFB0B1B2B3B4B5B6B7B8B9BABBBCBDBEBFC0C1C2C3C4C5C6C7C8C9CACBCCCDCECFD0D1D2D3D4D5D6D7D8D9DADBDCDDDEDFE0E1E2E3E4E5E6E7E8E9EAEBECEDEEEFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF";
        break;
    default: unreachable();
    }
    unsigned base2 = base * base;

    #define FORMAT_SIGNED_INTEGER(T, T_min, U, U_max, U_width) do {            \
            typeof(T) x = va_arg(*args, T);                                    \
            typeof(U) n;                                                       \
            if (x == T_min) {                                                  \
                n = (U)(-(x + (T)1)) + (U)1;                                   \
            } else {                                                           \
                n = (U)x;                                                      \
            }                                                                  \
            size_t buf_len = CHAR_BIT * U_width                                \
                + 2 /* prefix */                                               \
                + 1 /* negative sign */;                                       \
            char *buf = malloc(buf_len);                                       \
            if (!buf) {                                                        \
                return E_Writer_Malloc;                                        \
            }                                                                  \
            size_t i = buf_len;                                                \
            while (n >= base2) {                                               \
                typeof(U) d = n % base2;                                       \
                i -= 2;                                                        \
                memcpy(&buf[i], &digits2[2 * d], 2);                           \
                n /= base2;                                                    \
            }                                                                  \
            if (n < base) {                                                    \
                i -= 1;                                                        \
                buf[i] = digits1[n];                                           \
            } else {                                                           \
                i -= 2;                                                        \
                memcpy(&buf[i], &digits2[2 * n], 2);                           \
            }                                                                  \
            if (base == 2) {                                                   \
                i -= 2;                                                        \
                memcpy(&buf[i], "0b", 2);                                      \
            } else if (base == 8) {                                            \
                i -= 2;                                                        \
                memcpy(&buf[i], "0o", 2);                                      \
            } else if (base == 16) {                                           \
                i -= 2;                                                        \
                memcpy(&buf[i], "0x", 2);                                      \
            }                                                                  \
            if (x < 0) {                                                       \
                i -= 1;                                                        \
                buf[i] = '-';                                                  \
            }                                                                  \
            ret = Write_All(writer, &buf[i], buf_len - i);                     \
            if (ret != 0) {                                                    \
                return ret;                                                    \
            }                                                                  \
            free(buf);                                                         \
        } while (0)

    #define FORMAT_UNSIGNED_INTEGER(U, U_max, U_width) do {                    \
            typeof(U) n = va_arg(*args, U);                                    \
            size_t buf_len = CHAR_BIT * U_width                                \
                + 2 /* prefix */;                                              \
            char *buf = malloc(buf_len);                                       \
            if (!buf) {                                                        \
                return E_Writer_Malloc;                                        \
            }                                                                  \
            size_t i = buf_len;                                                \
            while (n >= base2) {                                               \
                typeof(U) d = n % base2;                                       \
                i -= 2;                                                        \
                memcpy(&buf[i], &digits2[2 * d], 2);                           \
                n /= base2;                                                    \
            }                                                                  \
            if (n < base) {                                                    \
                i -= 1;                                                        \
                buf[i] = digits1[n];                                           \
            } else {                                                           \
                i -= 2;                                                        \
                memcpy(&buf[i], &digits2[2 * n], 2);                           \
            }                                                                  \
            if (base == 2) {                                                   \
                i -= 2;                                                        \
                memcpy(&buf[i], "0b", 2);                                      \
            } else if (base == 8) {                                            \
                i -= 2;                                                        \
                memcpy(&buf[i], "0o", 2);                                      \
            } else if (base == 16) {                                           \
                i -= 2;                                                        \
                memcpy(&buf[i], "0x", 2);                                      \
            }                                                                  \
            ret = Write_All(writer, &buf[i], buf_len - i);                     \
            if (ret != 0) {                                                    \
                return ret;                                                    \
            }                                                                  \
            free(buf);                                                         \
        } while (0)

    switch (fmt.length) {
        case format_length_default:
            FORMAT_SIGNED_INTEGER(int, INT_MIN, unsigned int, UINT_MAX, INT_WIDTH);
            break;
        case format_length_unsigned:
            FORMAT_UNSIGNED_INTEGER(unsigned int, UINT_MAX, UINT_WIDTH);
            break;
        case format_length_long:
            FORMAT_SIGNED_INTEGER(long, LONG_MIN, unsigned long, ULONG_MAX, LONG_WIDTH);
            break;
        case format_length_ulong:
            FORMAT_UNSIGNED_INTEGER(unsigned long, ULONG_MAX, ULONG_WIDTH);
            break;
        case format_length_long_long:
            FORMAT_SIGNED_INTEGER(long long, LLONG_MIN, unsigned long long, ULLONG_MAX, LLONG_WIDTH);
            break;
        case format_length_ulong_long:
            FORMAT_UNSIGNED_INTEGER(unsigned long long, ULLONG_MAX, ULLONG_WIDTH);
            break;
        case format_length_usize:
            FORMAT_UNSIGNED_INTEGER(size_t, SIZE_MAX, SIZE_WIDTH);
            break;
        default:
            ret = E_Writer_Format_Length;
            break;
    }
    #undef FORMAT_SIGNED_INTEGER
    #undef FORMAT_UNSIGNED_INTEGER
    return ret;
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
        case 'b':
            out->specifier = format_specifier_binary;
            break;
        case 'd':
            out->specifier = format_specifier_decimal;
            break;
        case 'o':
            out->specifier = format_specifier_octal;
            break;
        case 'x':
            out->specifier = format_specifier_hex;
            break;
        case 'X':
            out->specifier = format_specifier_Hex;
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
    }else if (strncmp("l", str, length_modifier_len) == 0) {
        out->length = format_length_long;
    } else if (strncmp("ll", str, length_modifier_len) == 0) {
        out->length = format_length_long_long;
    }else if (strncmp("u", str, length_modifier_len) == 0) {
        out->length = format_length_unsigned;
    } else if (strncmp("ul", str, length_modifier_len) == 0) {
        out->length = format_length_ulong;
    } else if (strncmp("ull", str, length_modifier_len) == 0) {
        out->length = format_length_ulong_long;
    } else if (strncmp("uz", str, length_modifier_len) == 0) {
        out->length = format_length_usize;
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
                Format_String(writer, placeholder, &args);
                break;
            case format_specifier_binary:
            case format_specifier_octal:
            case format_specifier_hex:
            case format_specifier_Hex:
            case format_specifier_decimal:
                Format_Integer(writer, placeholder, &args);
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
