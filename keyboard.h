#pragma once

void keyb_init();
char code_to_char(char code);
void cursor_moveto(unsigned int strnum, unsigned int pos);
void clear_console();
void outb (unsigned short port, unsigned char data);
void outw (unsigned short port, unsigned short data);

void out_str(const char* ptr);
void getline(char *buff, int size);
void putline(const char* line);
void put_symb(char symb);
void clear_line();

#define ENTER 28
#define BACKSPACE 14

#define GLOB_BUFF_SIZE 80
#define CONSOLE_WIDTH 80
#define CONSOLE_HEIGHT 25
#define LINE_START 0