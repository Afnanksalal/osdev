ISO = os.iso
BUILD = build
KERNEL = $(BUILD)/kernel.bin
GRUB_DIR = /usr/lib/grub/i386-pc

KERNEL_OBJS = $(BUILD)/kernel.o
GAME_OBJS = $(BUILD)/game.o $(BUILD)/rendering.o $(BUILD)/input.o

all: $(ISO)

$(ISO): $(KERNEL)
	mkdir -p isodir/boot/grub
	cp $(KERNEL) isodir/boot/kernel.bin
	echo 'set timeout=0' > isodir/boot/grub/grub.cfg
	echo 'set default=0' >> isodir/boot/grub/grub.cfg
	echo 'menuentry "My OS" { multiboot /boot/kernel.bin }' >> isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) isodir --directory=$(GRUB_DIR)

$(KERNEL): bootloader/boot.asm $(KERNEL_OBJS) $(GAME_OBJS) linker.ld
	mkdir -p $(BUILD)
	nasm -f elf32 bootloader/boot.asm -o $(BUILD)/boot.o
	i686-elf-ld -m elf_i386 -T linker.ld -o $(KERNEL) $(BUILD)/boot.o $(KERNEL_OBJS) $(GAME_OBJS)

$(BUILD)/%.o: kernel/%.c
	mkdir -p $(BUILD)
	i686-elf-gcc -ffreestanding -m32 -nostdlib -mno-sse -mno-mmx -mno-80387 -mno-red-zone -fno-stack-check -fno-stack-protector -c $< -o $@

$(BUILD)/%.o: game/%.c
	mkdir -p $(BUILD)
	i686-elf-gcc -ffreestanding -m32 -nostdlib -mno-sse -mno-mmx -mno-80387 -mno-red-zone -fno-stack-check -fno-stack-protector -c $< -o $@

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -serial stdio

clean:
	rm -rf $(BUILD) isodir $(ISO)
