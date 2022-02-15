.code16
.intel_syntax noprefix

start:
    mov ax, cs 
    mov ds, ax
    mov ss, ax 
    mov sp, start 

init_console:
    mov ah, 0x00
    mov al, 0x03
    int 0x10
    mov dh, 0x00
    mov cur_line, dh
    mov ah, 0x02
    int 0x10

    mov si, msg_boot_done
    call print
kernel_init:
    mov ax, 0x00
    call reset_disk
    call load_kernel

    jmp switch_to_protected_mod

error:
    mov si, msg_error
    call print
    jmp $
reset_disk: 
    mov dl, ah
    mov ah, 0x00
    int 0x13
    jc reset_disk

    mov si, msg_reset_done
    call print
    ret

load_kernel:
    mov dl, 0x01   
    mov cx, 0x0001 
    mov ah, 0x02
    mov al, 0x12
    mov dh, 0x00 
    xor bx, bx
    mov es, bx
    mov ds, bx
    mov bx, 0x8000

    int 0x13

    mov dl, 0x01   
    mov cx, 0x0001 
    mov ah, 0x02
    mov al, 0x03 
    mov dh, 0x01 
    xor bx, bx
    mov es, bx
    mov ds, bx
    mov bx, 0xA400

    int 0x13

    jc error 

    mov si, msg_kernel_load_done
    call print
    ret

print:
      mov ah, 0x0E         

    .run:
      lodsb               
      cmp al, 0x00        
      je .done            
      int 0x10            
      jmp .run            

    .done:
      mov dh, [cur_line]
      inc dh
      mov [cur_line], dh
      mov ah, 0x02
      int 0x10
      ret  

switch_to_protected_mod:
    cli

    lgdt gdt_info ; 

    in al, 0x92
    or al, 2
    out 0x92, al
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp 0x8:protected_mode ; 

.code32
protected_mode:

    mov ax, 0x10
    mov es, ax
    mov ds, ax
    mov ss, ax

    call 0x8000

cur_line: .byte 0
msg_kernel_load_done: .asciz "Kernel loaded successfully"
msg_reset_done: .asciz "Disk reset successfully"
msg_boot_done: .asciz "Bootloader ran successfully"
msg_error: .asciz "Something went wrong"
gdt:
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0xff, 0xff, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00
    .byte 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00
gdt_info: 
    .word gdt_info - gdt 
    .word gdt, 0

.zero (512 -($ - start)- 2)
.byte 0x55, 0xAA;
