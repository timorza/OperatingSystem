export PATH=$PATH:/usr/local/i386elfgcc/bin


#COMPILING
nasm "Bootloader/bootloader.asm" -f bin -o "Binaries/bootloader.bin"
nasm "Bootloader/kernel_entry.asm" -f elf -o "Binaries/kernel_entry.o"
i386-elf-gcc -ffreestanding -m32 -g -c "Kernel/kernel.cpp" -o "Binaries/kernel.o"
nasm "Bootloader/padding.asm" -f bin -o "Binaries/padding.bin"
i386-elf-gcc -ffreestanding -m32 -g -c "Drivers/screenDriver.cpp" -o "Binaries/screenDriver.o"
i386-elf-gcc -ffreestanding -m32 -g -c "Hardware/IO.cpp" -o "Binaries/IO.o"
i386-elf-gcc -ffreestanding -m32 -g -c "Interrupts/IDT.cpp" -o "Binaries/IDT.o"
nasm "Interrupts/interrupts.asm" -f elf -o "Binaries/interrupts.o"
i386-elf-gcc -ffreestanding -m32 -g -c "Interrupts/isr.cpp" -o "Binaries/isr.o"
i386-elf-gcc -ffreestanding -m32 -g -c "Hardware/timer.cpp" -o "Binaries/timer.o"
i386-elf-gcc -ffreestanding -m32 -g -c "Memory/paging.cpp" -o "Binaries/paging.o"
i386-elf-gcc -ffreestanding -m32 -g -c "Kernel/Panic.cpp" -o "Binaries/Panic.o"
i386-elf-gcc -ffreestanding -m32 -g -c "Memory/memory.cpp" -o "Binaries/memory.o"
i386-elf-gcc -ffreestanding -m32 -g -c "Drivers/keyboard.cpp" -o "Binaries/keyboard.o"






#COMPILING

#LINKING
i386-elf-ld -o "Binaries/full_kernel.bin" -Ttext 0x1000 "Binaries/kernel_entry.o" "Binaries/kernel.o" "Binaries/screenDriver.o" "Binaries/IO.o" "Binaries/interrupts.o" "Binaries/IDT.o" "Binaries/isr.o" "Binaries/timer.o" "Binaries/paging.o" "Binaries/Panic.o" "Binaries/memory.o" "Binaries/keyboard.o" --oformat binary
#LINKING


cat "Binaries/bootloader.bin" "Binaries/full_kernel.bin" "Binaries/padding.bin"  > "Binaries/OS.bin"

qemu-system-x86_64 -drive format=raw,file="Binaries/OS.bin",index=0,if=floppy,  -m 4024M -no-reboot -no-shutdown -d guest_errors
