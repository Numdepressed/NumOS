#ifndef SCREEN_H
#define SCREEN_H

#define VIDEO_MEMORY 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0F
#define RED_ON_WHITE 0xF4

// Screen I/O ports
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

// Public kernel API
void clear_screen();
void kprint_at(char *message, int col, int row);
void kprint(char *message);
void kprint_backspace();
void set_cursor_offset(int offset);
int get_cursor_offset();
void set_text_color(int color);

// Private kernel functions
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);
int print_char(char c, int col, int row, char attr);
int get_screen_offset();
void set_screen_offset(int offset);
int handle_scrolling(int offset);

// Utility functions
void memory_copy(char *source, char *dest, int nbytes);

// Port I/O functions
unsigned char port_byte_in(unsigned short port);
void port_byte_out(unsigned short port, unsigned char data);

#endif