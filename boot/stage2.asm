[bits 16]

global _start
extern gdt_start

_start:
    call clear_screen
    call set_cursor_top_left

    mov si, msg
    call print_string

    ; Load kernel from disk
    mov si, kernel_load_msg
    call print_string
    call load_kernel

    mov si, gdt_msg
    call print_string

    jmp gdt_start

; Inline print functions (needed since we can't rely on external ones here)
print_string:
    mov ah, 0x0E
.loop:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .loop
.done:
    ret

clear_screen:
    mov ax, 0x0600
    mov bh, 0x07
    mov cx, 0x0000
    mov dx, 0x184F
    int 0x10
    ret

set_cursor_top_left:
    mov ah, 0x02
    mov bh, 0x00
    mov dh, 0x00
    mov dl, 0x00
    int 0x10
    ret

load_kernel:
    mov bx, 0x1000       ; Load kernel at 0x10000 (segment:offset = 0x1000:0x0000)
    mov es, bx
    mov bx, 0x0000
    mov dh, 10           ; Number of sectors (adjust as needed)
    mov cl, 0x03         ; Starting sector (sector 3, after MBR and stage2)
    call disk_load
    ret

disk_load:
    mov ah, 0x02
    mov al, dh
    mov ch, 0x00
    mov dh, 0x00
    mov dl, 0x80
    int 0x13
    jc disk_error
    ret

disk_error:
    mov si, disk_err_msg
    call print_string
    jmp $

msg db 'Stage 2 loaded', 13, 10, 0
kernel_load_msg db 'Loading kernel...', 13, 10, 0
gdt_msg db 'Loading GDT...', 13, 10, 0
disk_err_msg db 'Kernel load error!', 13, 10, 0