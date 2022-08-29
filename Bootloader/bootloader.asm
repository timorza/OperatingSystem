[org 0x7c00]

KERNEL_LOC equ 0x1000	;Physical location of kernel
mov [DISK_NUM],dl 	;The number of the disk,given by BIOS



main:

	mov bx,KERNEL_LOC	;Where to put Kernel
	mov cl, 2 		;Index of sector to start reading
	mov al, 20		;Sectors to read
	call read_from_disk	;Read disk

	;Switching to text mode and clear screen
	mov ah, 0
	mov al, 3
	int 0x10

	;Starting the switch to protected mode
	cli
	lgdt[GDT_descriptor]
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	jmp CODE_SEG:start_protected_mode
	jmp $


read_from_disk:
	;al - num of sectors, cl - index of sector
	push dx

	mov ah, 2 	;BIOS read sector function.
	mov dh, 0 	;Head Number
	mov ch, 0	;Cylinder number
	mov dl, [DISK_NUM]


	int 0x13
	pop dx
	jc error_disk

	ret

error_disk:
	;In case the disk reading is failed.
	mov bx, ERROR_DISK_MSG
	call print_string
	jmp $


print_string:
	;move the string to bx.
	mov ah, 0x0e
	mov al, [bx]
	cmp al, 0
	je return
	int 0x10 	;BIOS interrupt to print char.
	inc bx
	jmp print_string


return:
	ret


ERROR_DISK_MSG db "Disk read failed!", 0
DISK_NUM: db 0


GDT    null_descriptor:
        	dd 0;
        	dd 0


        data_descriptor:
        	dw 0xffff	;The limit.
        	dw 0
        	db 0
        	db 0b10010010	;The present, privilege, type and the type flags.
        	db 0b11001111	;The other flags.
		db 0

	code_desciptor:
        	dw 0xffff	;The limit.
        	dw 0
        	db 0
        	db 0b10011010	;The present, privilege, type and the type flags.
        	db 0b11001111	;The other flags.
        	db 0

GDT_end:

GDT_descriptor:
    	dw GDT_end - GDT_start - 1	;Size.
    	dd GDT_start			;Start.

CODE_SEG equ code_desciptor - GDT_start ;Costant.
DATA_SEG equ data_descriptor - GDT_start ;Costant.

halt:
	hlt

[bits 32]
start_protected_mode:
	mov ax, CODE_SEG
	mov [0x7777],ax ;access it in interrupts

	
	mov ax, DATA_SEG

	
	;Updating all the segment registers
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov ebp, 0x90000		; 32 bit stack base pointer
	mov esp, ebp

	jmp KERNEL_LOC
	
	hlt
	

times 510-($-$$) db 0
db 0x55, 0xaa
