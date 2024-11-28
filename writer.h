#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

// clang-format off

struct Writer {
    void *ctx;
    ssize_t (*write_fn)(void *ctx, const char *buf, size_t count);
};

static ssize_t
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

ssize_t
Write(struct Writer writer, const char *buf, size_t count)
{
    return writer.write_fn(writer.ctx, buf, count);
}

int
Write_All(struct Writer writer, const char *buf, size_t count)
{
    for (size_t i = 0; i < count;) {
        ssize_t written = Write(writer, buf, count);
        if (written < 0) {
            return written;
        }
        i += written;
    }
    return 0;
}

/// TODO: Implement
int
Print(struct Writer self, const char *format, ...)
{
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
        int ret = Write(writer, bytes, to_write);
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

// fn Write_Int(self: Self, comptime T: type, value: T, endian: std.builtin.Endian) anyerror!void {
//     var bytes: [@divExact(@typeInfo(T).int.bits, 8)]u8 = undefined;
//     mem.writeInt(std.math.ByteAlignedInt(@TypeOf(value)), &bytes, value, endian);
//     return self.writeAll(&bytes);
// }
//
// fn Write_Struct(self: Self, value: anytype) anyerror!void {
//     // Only extern and packed structs have defined in-memory layout.
//     comptime assert(@typeInfo(@TypeOf(value)).@"struct".layout != .auto);
//     return self.writeAll(mem.asBytes(&value));
// }
//
// fn Write_Struct_Endian(self: Self, value: anytype, endian: std.builtin.Endian) anyerror!void {
//     // TODO: make sure this value is not a reference type
//     if (native_endian == endian) {
//         return self.Write_Struct(value);
//     } else {
//         var copy = value;
//         mem.byteSwapAllFields(@TypeOf(value), &copy);
//         return self.Write_Struct(copy);
//     }
// }
//
// fn Write_File(self: Self, file: std.fs.File) anyerror!void {
//     // TODO: figure out how to adjust std lib abstractions so that this ends up
//     // doing sendfile or maybe even copy_file_range under the right conditions.
//     var buf: [4000]u8 = undefined;
//     while (true) {
//         const n = try file.Read_All(&buf);
//         try self.Write_All(buf[0..n]);
//         if (n < buf.len) return;
//     }
// }
