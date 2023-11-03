include Makefile.inc

KERNEL = kernel.bin
ELF = kernel.elf
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

SOURCES_DRIVERS = $(wildcard drivers/*.c)
OBJECTS_DRIVERS = $(SOURCES_DRIVERS:.c=.o)

SOURCES_INTERRUPTIONS = $(wildcard interruptions/*.c)
OBJECTS_INTERRUPTIONS = $(SOURCES_INTERRUPTIONS:.c=.o)

SOURCES_ASM = $(wildcard asm/*.asm)
OBJECTS_ASM = $(SOURCES_ASM:.asm=.o)

SOURCES_BUDDY_MM = $(wildcard memory_manager/*_buddy.c)
OBJECTS_BUDDY_MM = $(SOURCES_BUDDY_MM:.c=.o)

SOURCES_BITMAP_MM = $(wildcard memory_manager/*_bitmap.c)
OBJECTS_BITMAP_MM = $(SOURCES_BITMAP_MM:.c=.o)

LOADERSRC = loader.asm
LOADEROBJECT = $(LOADERSRC:.asm=.o)
STATICLIBS =

all: $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_INTERRUPTIONS) $(OBJECTS_BITMAP_MM)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_DRIVERS) $(OBJECTS_BITMAP_MM) $(OBJECTS_INTERRUPTIONS) $(OBJECTS_ASM) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o $(ELF) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_DRIVERS) $(OBJECTS_BITMAP_MM) $(OBJECTS_INTERRUPTIONS) $(OBJECTS_ASM) $(STATICLIBS)

buddy: $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_INTERRUPTIONS) $(OBJECTS_BUDDY_MM)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_DRIVERS) $(OBJECTS_INTERRUPTIONS) $(OBJECTS_BUDDY_MM) $(OBJECTS_ASM) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o $(ELF) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_DRIVERS) $(OBJECTS_INTERRUPTIONS) $(OBJECTS_BUDDY_MM) $(OBJECTS_ASM) $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -I. -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT): $(LOADERSRC)
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf asm/*.o *.o *.bin
	rm -rf drivers/*.o *.o *.bin
	rm -rf interruptions/*.o *.o *.bin
	rm -rf $(KERNEL) $(ELF)
	rm -rf memory_manager/*.o *.o *.bin

.PHONY: all clean buddy
