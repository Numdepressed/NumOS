[org 0x7C00]
bits 16

start:
    call clear_screen
    call set_cursor_top_left

    mov si, msg
    call print_string

    mov bx, 0x7E00       ; Load address for Stage 2
    mov dh, 1            ; Number of sectors to read
    call load_stage2

    jmp 0x0000:0x7E00    ; Far jump to Stage 2

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

load_stage2:
    mov ah, 0x02
    mov al, dh
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02
    mov dl, 0x80
    int 0x13
    jc disk_error
    ret

disk_error:
    mov si, err_msg
    call print_string
    hlt

msg db 'MBR loaded!', 0
err_msg db 'Disk read error!', 0

times 510 - ($ - $$) db 0
dw 0xAA55
