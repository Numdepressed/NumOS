#include "keyboard.h"

/* Keyboard state variables */
int shift_pressed = 0;
int caps_lock_on = 0;

/* Keyboard buffer */
static keyboard_buffer_t kb_buffer = {0};

/* Command line state */
static command_line_t cmd_line = {0};

/* Text protection variables */
static int protected_end_offset = 0;  // Marks end of protected text
static int input_start_offset = 0;    // Marks start of current input line

/* Scancode to ASCII mapping */
const char scancode_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, /* 29 - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0, /* Right shift */
    '*',
    0,  /* Alt */
    ' ', /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

/* Shifted characters */
const char scancode_ascii_shift[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, /* 29 - Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, /* Left shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0, /* Right shift */
    '*',
    0,  /* Alt */
    ' ', /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

void init_keyboard() {
    /* Initialize keyboard buffer */
    kb_buffer.head = 0;
    kb_buffer.tail = 0;
    kb_buffer.count = 0;
    
    /* Initialize command line */
    init_command_line();
    
    /* Enable keyboard interrupts */
    /* This will be implemented when we add interrupt handling */
}

void init_command_line() {
    cmd_line.length = 0;
    cmd_line.buffer[0] = '\0';
    set_prompt_position();
}

void set_prompt_position() {
    /* Get current cursor position as the prompt start */
    int offset = get_cursor_offset();
    cmd_line.prompt_row = get_offset_row(offset);
    cmd_line.prompt_col = get_offset_col(offset);
    
    /* Mark this as the start of input area */
    input_start_offset = offset;
}

void mark_protected_text() {
    /* Mark current cursor position as end of protected text */
    protected_end_offset = get_cursor_offset();
}

void keyboard_callback() {
    /* Read from the keyboard's data buffer */
    unsigned char scancode = port_byte_in(0x60);
    handle_keyboard_input(scancode);
}

void handle_keyboard_input(int scancode) {
    /* Handle special keys */
    if (scancode == LSHIFT || scancode == RSHIFT) {
        shift_pressed = 1;
        return;
    }
    
    if (scancode == LSHIFT_REL || scancode == RSHIFT_REL) {
        shift_pressed = 0;
        return;
    }
    
    if (scancode == CAPS_LOCK) {
        caps_lock_on = !caps_lock_on;
        return;
    }
    
    /* Handle key releases (scancode >= 0x80) */
    if (scancode >= 0x80) {
        return; /* Ignore key releases for now */
    }
    
    /* Convert scancode to ASCII */
    char ascii = get_scancode_ascii(scancode, shift_pressed);
    
    if (ascii != 0) {
        /* Handle backspace */
        if (ascii == '\b') {
            /* Only allow backspace if we have characters to delete 
               AND we're not trying to delete protected text */
            int current_offset = get_cursor_offset();
            
            if (cmd_line.length > 0 && current_offset > input_start_offset) {
                /* Remove character from command buffer */
                cmd_line.length--;
                cmd_line.buffer[cmd_line.length] = '\0';
                
                /* Move cursor back and clear the character */
                kprint_backspace();
            }
            /* If we're at or before the input start, ignore the backspace */
            return;
        }
        
        /* Handle enter */
        if (ascii == '\n') {
            /* Null-terminate the command */
            cmd_line.buffer[cmd_line.length] = '\0';
            
            /* Process the command */
            kprint("\n");
            if (cmd_line.length > 0) {
                process_command(cmd_line.buffer);
            }
            
            /* Reset command line for next input */
            kprint("> ");
            cmd_line.length = 0;
            cmd_line.buffer[0] = '\0';
            set_prompt_position();
            return;
        }
        
        /* Handle regular characters */
        if (cmd_line.length < COMMAND_BUFFER_SIZE - 1) {
            /* Add to command buffer */
            cmd_line.buffer[cmd_line.length] = ascii;
            cmd_line.length++;
            cmd_line.buffer[cmd_line.length] = '\0';
            
            /* Print the character */
            char str[2] = {ascii, '\0'};
            kprint(str);
        }
    }
}

void process_command(char *command) {
    /* Simple command processing */
    if (command[0] == '\0') {
        /* Empty command, do nothing */
        return;
    }
    
    /* Compare strings manually since we don't have strcmp */
    int is_help = 1;
    char *help_cmd = "help";
    for (int i = 0; i < 4; i++) {
        if (command[i] != help_cmd[i]) {
            is_help = 0;
            break;
        }
    }
    if (command[4] != '\0') is_help = 0;
    
    int is_clear = 1;
    char *clear_cmd = "clear";
    for (int i = 0; i < 5; i++) {
        if (command[i] != clear_cmd[i]) {
            is_clear = 0;
            break;
        }
    }
    if (command[5] != '\0') is_clear = 0;
    
    if (is_help) {
        kprint("Available commands:\n");
        kprint("  help  - Show this help message\n");
        kprint("  clear - Clear the screen\n");
        /* Mark the help text as protected */
        mark_protected_text();
    } else if (is_clear) {
        clear_screen();
        kprint("NumOS Kernel v0.00.1\n");
        kprint("System initialized\n");
        /* Mark the kernel banner as protected */
        mark_protected_text();
    } else {
        kprint("Unknown command: ");
        kprint(command);
        kprint("\n");
        kprint("Type 'help' for available commands\n");
        /* Mark error message as protected */
        mark_protected_text();
    }
}

char get_scancode_ascii(int scancode, int shift) {
    if (scancode > SC_MAX) return 0;
    
    if (shift) {
        return scancode_ascii_shift[scancode];
    } else {
        char c = scancode_ascii[scancode];
        
        /* Handle caps lock for letters */
        if (caps_lock_on && c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
        
        return c;
    }
}

void keyboard_buffer_put(char c) {
    if (kb_buffer.count < KEYBOARD_BUFFER_SIZE) {
        kb_buffer.buffer[kb_buffer.head] = c;
        kb_buffer.head = (kb_buffer.head + 1) % KEYBOARD_BUFFER_SIZE;
        kb_buffer.count++;
    }
}

char keyboard_buffer_get() {
    if (kb_buffer.count > 0) {
        char c = kb_buffer.buffer[kb_buffer.tail];
        kb_buffer.tail = (kb_buffer.tail + 1) % KEYBOARD_BUFFER_SIZE;
        kb_buffer.count--;
        return c;
    }
    return 0;
}

int keyboard_buffer_empty() {
    return kb_buffer.count == 0;
}

char keyboard_getchar() {
    /* Poll for keyboard input */
    while (keyboard_buffer_empty()) {
        /* Check if keyboard data is available */
        if (port_byte_in(0x64) & 0x01) {
            keyboard_callback();
        }
    }
    return keyboard_buffer_get();
}
