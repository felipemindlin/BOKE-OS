#!/bin/bash
if [[ "$1" = "gdb" ]]; then
    qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 2048 -s -S
else
    qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 2048
fi