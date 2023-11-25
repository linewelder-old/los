#!/bin/sh

set -e

make kernel
qemu-system-i386 -kernel los.bin -hda test.iso
