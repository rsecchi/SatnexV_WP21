#!/bin/bash

echo "start qemu"
KERNEL=vmlinuz-4.4.0-116-generic

VMDIR=/home/raffaello/workspace/debian_vm/debian/linux-4.4/arch/x86_64/boot/


# Using this function creates a VM which is accessible using a local port forward.
# QEMU listens on a local socket which is forwared to ssh into the VM.
connect_using_usermode () {

        qemu-system-x86_64 \
            -m 1024M \
            -hda debian1.raw \
            -soundhw hda \
            -k en-gb \
            -initrd /boot/initrd.img-4.15.0-112-generic \
            -kernel  $VMDIR/bzImage \
            -append "root=/dev/sda1" \
            -curses \
            -gdb tcp::5022 \
            -device e1000,netdev=eth0 \
            -netdev user,id=eth0,hostfwd=tcp::5555-:22
}

connect_using_tap () {

        qemu-system-x86_64 \
            -m 1024M \
            -hda debian1.raw \
            -soundhw hda \
            -k en-gb \
            -initrd /boot/initrd.img-4.15.0-112-generic \
            -kernel  $VMDIR/bzImage \
            -append "root=/dev/sda1" \
            -curses \
            -gdb tcp::5022 \
            -netdev tap,id=net0,ifname=vmtap1 \
            -net nic,netdev=net0,macaddr=52:54:00:11:00:01
}

connect_using_tap


