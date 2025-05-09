GCCPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -fcheck-new
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = 	obj/loader.o \
			obj/gdt.o \
			obj/memorymanagement.o \
			obj/drivers/driver.o \
			obj/hardwarecommunication/port.o \
			obj/hardwarecommunication/interruptstubs.o \
			obj/hardwarecommunication/interrupts.o \
			obj/syscalls.o \
			obj/multitasking.o \
			obj/drivers/amd_am79c973.o \
			obj/hardwarecommunication/pci.o \
			obj/drivers/keyboard.o \
			obj/drivers/mouse.o \
			obj/drivers/vga.o \
			obj/drivers/ata.o \
			obj/filesystem/msdospart.o \
			obj/filesystem/fat.o \
			obj/gui/widget.o \
			obj/gui/window.o \
			obj/gui/desktop.o \
			obj/net/etherframe.o \
			obj/net/arp.o \
			obj/net/ipv4.o \
			obj/net/icmp.o \
			obj/net/udp.o \
			obj/net/tcp.o \
			obj/kernel.o

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	gcc $(GCCPARAMS) -o $@ -c $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

install: mykernel.bin
	sudo cp $< /boot/mykernel.bin

mykernel.iso: mykernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp mykernel.bin iso/boot/mykernel.bin
	echo 'set timeout=0' 					>> iso/boot/grub/grub.cfg
	echo 'set default=0' 					>> iso/boot/grub/grub.cfg
	echo '' 								>> iso/boot/grub/grub.cfg
	echo 'menuentry "MyOS" {' 				>> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/mykernel.bin' 	>> iso/boot/grub/grub.cfg
	echo '	boot' 							>> iso/boot/grub/grub.cfg
	echo '}' 								>> iso/boot/grub/grub.cfg
	grub-mkrescue --output=mykernel.iso iso
	rm -rf iso

run: mykernel.iso
	(killall VirtualBoxVM && sleep 1) || true
	VBoxManage startvm MyOS &

.PHONY: clean
clean:
	rm -rf iso
	rm -rf obj mykernel.bin mykernel.iso
