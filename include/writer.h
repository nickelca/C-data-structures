#ifndef WRITER_H
#define WRITER_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

// clang-format off

enum E_Writer {
    /// Full count of bytes was not successfully written
    E_Writer_Short_Count = 1,
    /// Format placeholder is missing opening {
    E_Writer_Placeholder_Start,
    /// Format placeholder is missing closing }
    E_Writer_Placeholder_End,
};

struct Writer {
    void *ctx;
    size_t (*write_fn)(void *ctx, const char *buf, size_t count);
};

/// Get writer pointing to stdout
struct Writer
Get_STD_Out(void);

/// Write up to count bytes from buf
///
/// TODO: Find proper error values
size_t
Write(struct Writer writer, const char *buf, size_t count);

/// Write count bytes from buf, erroring if full count could not be written
///
/// On success returns 0.
/// On failure returns non-zero value.
int
Write_All(struct Writer writer, const char *buf, size_t count);

/// Print a formatted string.
/// Placeholders follow the format:
/// {[length modifier][specifier]:[fill][alignment][width].[precision]}.
/// [length modifier] modifies the length of the type specified by [specifier]
/// [specifier] determines how an argument should be formatted.
/// [fill] is a single char which is used to pad the text.
/// [alignment] is one of '<', '^', '>' to make the text left-, center-, or
/// right-aligned, respectively.
/// [width] is the total width of the field.
/// [precision] specifies how many decimals a formatted floating point number
/// should have.
///
/// [length modifier] can be one of:
/// s    short.
/// l    long.
/// ll   long long.
/// [specifier] can be one of:
/// d    Print number as decimal (base 10).
/// x    Print number as hexadecimal (base 16).
/// o    Print number as octal (base 8).
/// c    Print number as a character.
///
/// To print literal { and }, escape them by writing them twice, e.g. {{ or }}.
///
/// On success returns 0.
/// On failure returns non-zero value.
///
/// Undefined Behavior occurs if [specifier] does not match the type of the
/// corresponding argument.
///
/// TODO: Implement
int
Print(struct Writer writer, const char *format, ...);

/// Write a single char.
///
/// On success returns 0.
/// On failure returns non-zero value.
int
Write_Byte(struct Writer writer, char byte);

/// Write a single char N times.
///
/// On success returns 0.
/// On failure returns non-zero value.
int
Write_Byte_N_Times(struct Writer writer, char byte, size_t n);

/// Write a buffer of characters N times. If full amount of bytes could not be
/// written, this function fails.
///
/// On success returns 0.
/// On failure returns non-zero value.
int
Write_Bytes_N_Times(struct Writer writer, const char *bytes, size_t count, size_t n);

#endif
