#!/bin/bash

sudo mount -o loop dev_kernel_grub.img /media/floppy/
sudo cp kernel.bin /media/floppy/
sudo umount /media/floppy/