CC=g++
CFLAGS=-ffreestanding -m32

kernel: kernel.o intr.o keyboard.o utils.o
	ld --oformat binary -Ttext 0x8000 -o kernel.bin --entry=kmain -m elf_i386 kernel.o intr.o keyboard.o utils.o

bootsect: bootsect.o
	ld -Ttext 0x7c00 --oformat binary -m elf_i386 -o bootsect.bin bootsect.o

bootsect.o:
	as --32 -nostartfiles -o bootsect.o bootsect.asm

kernel.o: kernel.c
	$(CC) $(CFLAGS) -o kernel.o -c kernel.c

intr.o: intr.c
	$(CC) $(CFLAGS) -o intr.o -c intr.c

keyboard.o: keyboard.c
	$(CC) $(CFLAGS) -o keyboard.o -c keyboard.c

utils.o: utils.c
	$(CC) $(CFLAGS) -o utils.o -c utils.c

clean:
	rm -rf *.o kernel