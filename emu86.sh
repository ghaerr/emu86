#!/bin/bash

# Run ELKS in EMU86 (basic 8086 emulator)
# EMU86 is part of the cross tools

# For ELKS ROM Configuration:
# ELKS must be configured minimaly with 'cp emu86-rom.config .config'
# This uses headless console, HLT on idle, ROM filesystem.

# First build ELKS with kernel and root FS in ROM
# Kernel image @ segment 0xE000 (top of 1024K address space)
# Root filesystem @ segment 0x8000 (assumes 512K RAM & 512K ROM)
# Skip the INT 19h bootstrap in the kernel image (+0x14)

ELKSDIR=../elks-gh/

#exec ./emu86 -i -w 0xe0000 -f Image -w 0x80000 -f romfs.bin
exec ./emu86 \
	-C 0xe062 -D 0x00c0 \
    -w 0xe0000 -f ${ELKSDIR}elks/arch/i86/boot/Image \
    -w 0x80000 -f ${ELKSDIR}image/romfs.bin \
    -S ${ELKSDIR}elks/arch/i86/boot/system.sym

#exec emrun --serve_after_close emu86.html -- -v3 -w 0x10000 -x 0x1000:0x34 -f romfs.bin

# For ELKS disk image Configuration:
# ELKS must be configured with 'cp emu86-disk.config .config'
# This uses headless console, HLT on idle, no CONFIG_IDE_PROBE

exec ./emu86 -v0 -I ../elks-gh/image/fd1440.bin -M ../elks-gh/elks/arch/i86/boot/system-nm.map
#exec ./emu86 -v0 -I fd.img
#exec ./emu86 -I ../elks-gh/image/fd1440.bin -I ../elks-gh/image/hd32-minix.bin ${1+"$@"}
#exec ./emu86 -I ../elks-gh/image/fd360-minix.bin ${1+"$@"}
#exec ./emu86 -I ../elks-gh/image/fd1440-fat.bin ${1+"$@"}
#exec ./emu86 -i -I ../elks-gh/image/hd32-minix.bin ${1+"$@"}
#exec ./emu86 -I ../elks-gh/image/hd32mbr-minix.bin ${1+"$@"}
exec ./emu86 -I ../elks-gh/image/hd32-fat.bin ${1+"$@"}
