CPP = gcc
CPP_OPTIONS = -fno-asynchronous-unwind-tables -m32 -s -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions -fno-rtti -fno-stack-protector -fleading-underscore

all: kernel.bin

clean:
	rm -f *.o

start.o: start.asm gdt_low.asm idt_low.asm irq_low.asm
	nasm -f aout -o start.o start.asm

utils.o: utils.C utils.H
	$(CPP) $(CPP_OPTIONS) -c -o utils.o utils.C

assert.o: assert.C assert.H
	$(CPP) $(CPP_OPTIONS) -c -o assert.o assert.C


# ==== VARIOUS LOW-LEVEL STUFF =====

gdt.o: gdt.C gdt.H
	$(CPP) $(CPP_OPTIONS) -c -o gdt.o gdt.C

machine.o: machine.C machine.H
	$(CPP) $(CPP_OPTIONS) -c -o machine.o machine.C

machine_low.o: machine_low.asm machine_low.H
	nasm -f aout -o machine_low.o machine_low.asm


# ==== EXCEPTIONS AND INTERRUPTS =====

idt.o: idt.C idt.H
	$(CPP) $(CPP_OPTIONS) -c -o idt.o idt.C

irq.o: irq.C irq.H
	$(CPP) $(CPP_OPTIONS) -c -o irq.o irq.C

exceptions.o: exceptions.C exceptions.H
	$(CPP) $(CPP_OPTIONS) -c -o exceptions.o exceptions.C

interrupts.o: interrupts.C interrupts.H
	$(CPP) $(CPP_OPTIONS) -c -o interrupts.o interrupts.C


# ==== DEVICES =====

console.o: console.C console.H
	$(CPP) $(CPP_OPTIONS) -c -o console.o console.C

simple_timer.o: simple_timer.C simple_timer.H
	$(CPP) $(CPP_OPTIONS) -c -o simple_timer.o simple_timer.C


# ==== MEMORY =====

paging_low.o: paging_low.asm paging_low.H
	nasm -f aout -o paging_low.o paging_low.asm

page_table.o: page_table.C page_table.H paging_low.H
	$(CPP) $(CPP_OPTIONS) -c -o page_table.o page_table.C
	
frame_pool.o: frame_pool.C frame_pool.H
	$(CPP) $(CPP_OPTIONS) -c -o frame_pool.o frame_pool.C

vm_pool.o: vm_pool.C vm_pool.H
	$(CPP) $(CPP_OPTIONS) -c -o vm_pool.o vm_pool.C
	
# ==== KERNEL MAIN FILE =====

kernel.o: kernel.C console.H simple_timer.H page_table.H
	$(CPP) $(CPP_OPTIONS) -c -o kernel.o kernel.C


kernel.bin: start.o utils.o kernel.o assert.o console.o gdt.o idt.o irq.o exceptions.o interrupts.o \
   simple_timer.o paging_low.o page_table.o frame_pool.o vm_pool.o machine.o machine_low.o
	ld -m elf_i386 -T linker.ld -o kernel.bin start.o utils.o kernel.o assert.o console.o gdt.o idt.o \
   exceptions.o irq.o interrupts.o simple_timer.o  paging_low.o page_table.o \
   frame_pool.o vm_pool.o machine.o machine_low.o
	
