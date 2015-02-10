# Replace "/mnt/floppy" with the whatever directory is appropriate.
#sudo umount /mnt/floppy
sudo mount -o loop dev_kernel_grub.img /mnt/floppy
sudo cp kernel.bin /mnt/floppy
sudo umount /mnt/floppy
