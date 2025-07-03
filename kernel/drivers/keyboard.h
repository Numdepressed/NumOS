#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "screen.h"

#define BACKSPACE 0x0E
#define ENTER 0x1C
#define LSHIFT 0x2A
#define RSHIFT 0x36
#define LSHIFT_REL 0xAA
#define RSHIFT_REL 0xB6
#define CAPS_LOCK 0x3A

#define SC_MAX 57

/* Keyboard buffer */
#define KEYBOARD_BUFFER_SIZE 256
#define COMMAND_BUFFER_SIZE 256

typedef struct {
    char buffer[KEYBOARD_BUFFER_SIZE];
    int head;
    int tail;
    int count;
} keyboard_buffer_t;

/* Command line management */
typedef struct {
    char buffer[COMMAND_BUFFER_SIZE];
    int length;
    int prompt_col;
    int prompt_row;
} command_line_t;

/* Function declarations */
void init_keyboard();
void keyboard_callback();
char get_scancode_ascii(int scancode, int shift);
void handle_keyboard_input(int scancode);
char keyboard_getchar();
int keyboard_buffer_empty();
void keyboard_buffer_put(char c);
char keyboard_buffer_get();

/* Command line functions */
void init_command_line();
void set_prompt_position();
void process_command(char *command);

/* Text protection functions */
void mark_protected_text();

/* Keyboard state */
extern int shift_pressed;
extern int caps_lock_on;

#endif