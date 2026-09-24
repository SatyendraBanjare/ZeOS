#!/usr/bin/env python3
"""Pack a directory into a ZeOS initrd image.

Layout (little endian):
    uint32  file_count
    file_count * header (76 bytes):
        uint8   magic (0xBF)
        char    name[64]      path relative to the directory, '/' separated, NUL padded
        3 bytes padding
        uint32  offset        absolute offset of the data inside the image
        uint32  length
    file data
"""
import os
import struct
import sys

MAGIC = 0xBF
NAME_LEN = 64
HEADER = struct.Struct("<B64s3xII")   # 76 bytes
assert HEADER.size == 76


def collect(root):
    files = []
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames.sort()
        for fn in sorted(filenames):
            full = os.path.join(dirpath, fn)
            rel = os.path.relpath(full, root).replace(os.sep, "/")
            if len(rel.encode()) >= NAME_LEN:
                sys.exit("make_initrd: path too long (max %d): %s" % (NAME_LEN - 1, rel))
            files.append((rel, full))
    return files


def main():
    if len(sys.argv) != 3:
        sys.exit("usage: make_initrd.py <directory> <output image>")
    root, out = sys.argv[1], sys.argv[2]
    files = collect(root)

    offset = 4 + HEADER.size * len(files)
    table, blobs = b"", b""
    for rel, full in files:
        with open(full, "rb") as f:
            data = f.read()
        table += HEADER.pack(MAGIC, rel.encode(), offset + len(blobs), len(data))
        blobs += data

    with open(out, "wb") as f:
        f.write(struct.pack("<I", len(files)) + table + blobs)
    print("initrd: %d file(s), %d bytes -> %s" % (len(files), 4 + len(table) + len(blobs), out))


if __name__ == "__main__":
    main()
