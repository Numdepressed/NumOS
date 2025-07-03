[bits 16]

global gdt_start
extern print_string, clear_screen, set_cursor_top_left

gdt_start:
    ; Message 1: Starting GDT setup
    mov si, msg
    call print_string

    ; Set up GDT
    call setup_gdt
    
    ; Message 2: About to switch to protected mode
    mov si, switching_msg
    call print_string

    ; Switch to protected mode (this should be the LAST real mode code)
    call switch_to_pm

setup_gdt:
    lgdt [gdt_descriptor]
    ret

switch_to_pm:
    cli                     ; Disable interrupts
    
    ; Enable A20 line (simplified method)
    in al, 0x92
    or al, 2
    out 0x92, al
    
    ; Set PE bit in CR0
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    ; Far jump to flush pipeline and enter protected mode
    ; After this jump, we're in 32-bit protected mode
    jmp CODE_SEG:protected_mode_start

[bits 32]
; *** PROTECTED MODE CODE STARTS HERE ***
; NO MORE BIOS INTERRUPTS AFTER THIS POINT!

protected_mode_start:
    ; Set up segment registers for protected mode
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Set up stack
    mov ebp, 0x90000
    mov esp, ebp
    
    ; Clear screen in protected mode
    call clear_screen_pm
    
    ; Print success message in protected mode
    mov esi, pm_msg
    call print_string_pm
    
    ; Jump to kernel entry point
    jmp 0x10000

; Protected mode screen functions
clear_screen_pm:
    mov edi, 0xB8000  ; Video memory start
    mov ecx, 80 * 25  ; 80 columns * 25 rows
    mov ax, 0x0F20    ; White space on black background
    rep stosw         ; Fill screen with spaces
    ret

; Protected mode print function
print_string_pm:
    mov edi, 0xB8000  ; Video memory
pm_print_loop:
    mov al, [esi]
    cmp al, 0
    je pm_print_done
    mov [edi], al           ; Character
    mov byte [edi + 1], 0x0F ; White on black attribute
    add edi, 2
    inc esi
    jmp pm_print_loop
pm_print_done:
    ret

; GDT Table
gdt_start_table:
    ; Null descriptor
    dq 0x0

gdt_code:
    ; Code segment descriptor
    dw 0xFFFF    ; Limit (bits 0-15)
    dw 0x0       ; Base (bits 0-15)
    db 0x0       ; Base (bits 16-23)
    db 10011010b ; 1st flags, type flags
    db 11001111b ; 2nd flags, Limit (bits 16-19)
    db 0x0       ; Base (bits 24-31)

gdt_data:
    ; Data segment descriptor
    dw 0xFFFF    ; Limit (bits 0-15)
    dw 0x0       ; Base (bits 0-15)
    db 0x0       ; Base (bits 16-23)
    db 10010010b ; 1st flags, type flags
    db 11001111b ; 2nd flags, Limit (bits 16-19)
    db 0x0       ; Base (bits 24-31)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start_table - 1  ; Size of GDT
    dd gdt_start_table                ; Start address of GDT

; Segment selector constants
CODE_SEG equ gdt_code - gdt_start_table
DATA_SEG equ gdt_data - gdt_start_table

; Messages
msg db 'Setting up GDT...', 13, 10, 0
switching_msg db 'Switching to Protected Mode...', 13, 10, 0
pm_msg db 'Protected Mode Active! Jumping to kernel...', 0