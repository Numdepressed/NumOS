#include "kernel.h"
#include "drivers/screen.h"
#include "drivers/keyboard.h"

void kmain() {
    // Initialize keyboard system
    init_keyboard();
    
    // Clear screen
    clear_screen();
    
    // Print basic messages
    kprint("NumOS Kernel v0.00.1\n");
    kprint("System initialized\n");
    
    // Mark the kernel banner as protected text
    mark_protected_text();
    
    // Print prompt
    kprint("> ");
    
    // Set up the command line properly
    set_prompt_position();
    
    // Main keyboard loop using our proper keyboard handler
    while (1) {
        // Check if keyboard data is available
        if (port_byte_in(0x64) & 0x01) {
            keyboard_callback();
        }
    }
}