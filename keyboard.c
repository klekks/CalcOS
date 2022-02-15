#include "intr.h"
#include "keyboard.h"
#include "utils.h"

int shifted = 0;

char scancodes_to_char[128] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8',       // 0-9
    '9', '0', '-', '=', 0, 0, 'q', 'w', 'e', 'r',       // 10-19
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,    // 20-29
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',   // 30-39
    '\'', '`', 0, 0, 'z', 'x', 'c', 'v', 'b', 'n',      // 40-49
    'm', ',', '.', '/', '*', '*', 0,' ', 0, 0,          // 50-59
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                       // 60-69
    0, '7', '8', '9', '-', '4', '5', '6', '+', '1',     // 70-79
    '2', '3', 0, '.', 0, 0, 0, 0, 0, 0,                 // 80-89
    0, 0, '0', 0, 0, 0, 0, 0, 0, 0,                     // 90-99
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                       // 100-109
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                       // 110-120
    0, 0, 0, 0, 0, 0, 0, 0                              // 120 -127
};

char scancodes_to_shifted_chars[128]  = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*',       // 0-9
    '(', ')', '_', '+', 0, 0, 'Q', 'W', 'E', 'R',       // 10-19
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,       // 20-29
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',   // 30-39
    '"', '~', 0, 0, 'Z', 'X', 'C', 'V', 'B', 'N',      // 40-49
    'M', '<', '>', '?', '*', 0, 0,' ', 0, 0,            // 50-59
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                       // 60-69
    0, '7', '8', '9', '-', '4', '5', '6', '+', '1',     // 70-79
    '2', '3', '0', '.', 0, 0, 0, 0, 0, 0,                 // 80-89
    0, 0, '0', 0, 0, 0, 0, 0, 0, 0,                     // 90-99
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                       // 100-109
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                       // 110-120
    0, 0, 0, 0, 0, 0, 0, 0                              // 120 -127
};

void keyb_handler();
void next_line();

static inline unsigned char inb (unsigned short port) // Чтение из порта
{
    unsigned char data;
    asm volatile ("inb %w1, %b0" : "=a" (data) : "Nd" (port));
    return data;
}

void outb (unsigned short port, unsigned char data) // Запись
{
    asm volatile ("outb %b0, %w1" : : "a" (data), "Nd" (port));
}

void outw (unsigned short port, unsigned short data) // Запись
{
    asm volatile ("outw %w0, %w1" : : "a" (data), "Nd" (port));
}

void on_key(char);
void keyb_process_keys();

#define PIC1_PORT (0x20)
void keyb_init()
{
    // Регистрация обработчика прерывания
    intr_reg_handler(0x09, GDT_CS, 0x80 | IDT_TYPE_INTR, keyb_handler);
    // segm_sel=0x8, P=1, DPL=0, Type=Intr
    // Разрешение только прерываний клавиатуры от контроллера 8259
    outb(PIC1_PORT + 1, 0xFF ^ 0x02); // 0xFF - все прерывания, 0x02 - бит IRQ1 (клавиатура).
    // Разрешены будут только прерывания, чьи биты установлены в 0
}

void keyb_handler()
{
    asm("pusha");
    // Обработка поступивших данных
    keyb_process_keys();
    // Отправка контроллеру 8259 нотификации о том, что прерываниe обработано
    outb(PIC1_PORT, 0x20);
    asm( "popa "    "\n\t" 
         "leave"    "\n\t" 
         "iret"     "\n\t" 
        );
}

void key_handler(char code) {
    switch(code) {
        case 42:
            shifted = 1;
            return;
        case -86:
            shifted = 0; return;
        case 58:
            shifted = shifted ? 0 : 1; return;
        case 28:
            break; // enter
    };
}

void keyb_process_keys()
{
// Проверка что буфер PS/2 клавиатуры не пуст (младший бит присутствует)
    if (inb(0x64) & 0x01) {
            char scan_code;
            unsigned char state;
            scan_code = inb(0x60); // Считывание символа с PS/2 клавиатуры

            key_handler(scan_code);

            if (scan_code < 128) // Скан-коды выше 128 - это отпускание клавиши
                on_key(scan_code);
    }
}

char code_to_char(char code) {
    if (code < 0) return 0;
    if (shifted) return scancodes_to_shifted_chars[code];
    else return scancodes_to_char[code];
}

#define VIDEO_BUF_PTR (0xb8000 + 2*80*CURR_CONSOLE_LINE)
#define CURSOR_PORT (0x3D4)

static char scaning = 0;
char GLOB_BUFF[GLOB_BUFF_SIZE];
int NEXT_SYMB = 0;

int CURR_CONSOLE_LINE = 0;
int CURR_LINE_SYMB = LINE_START;
int OUTPUT_COLOR = 0x06;

void on_key(char scan_code) {

    char symb = code_to_char(scan_code);

    if (scaning) 
    {
        if (scan_code == ENTER) 
        {
            scaning = 0;
            GLOB_BUFF[NEXT_SYMB++] = '\0';
            next_line();
        }
        else if (scan_code == BACKSPACE) 
        {
            if (CURR_LINE_SYMB) CURR_LINE_SYMB--;
            if (NEXT_SYMB) NEXT_SYMB--;
            put_symb(' ');
            CURR_LINE_SYMB--;
            cursor_moveto(CURR_CONSOLE_LINE, CURR_LINE_SYMB);

        }
        else if (symb && scan_code > 0)
        {
            put_symb(symb);
            GLOB_BUFF[NEXT_SYMB++] = symb;
            GLOB_BUFF[NEXT_SYMB] = '\0';
            if (NEXT_SYMB >= 80) scaning = 0;
        }
    }
}

void cursor_moveto(unsigned int strnum, unsigned int pos)
{
    unsigned short new_pos = (strnum * CONSOLE_WIDTH) + pos;
    outb(CURSOR_PORT, 0x0F);
    outb(CURSOR_PORT + 1, (unsigned char)(new_pos & 0xFF));
    outb(CURSOR_PORT, 0x0E);
    outb(CURSOR_PORT + 1, (unsigned char)( (new_pos >> 8) & 0xFF));
}

void next_line() 
{
    CURR_CONSOLE_LINE = (CURR_CONSOLE_LINE + 1) % CONSOLE_HEIGHT;
    CURR_LINE_SYMB = LINE_START;
    clear_line();
}

void clear_line()
{
    unsigned char* videobuff = (unsigned char*) VIDEO_BUF_PTR;
    videobuff += 2 * CURR_CONSOLE_LINE * CONSOLE_WIDTH;
    for (int i = 0; i < CONSOLE_WIDTH; i++) 
    {
        videobuff[2*i] = ' ';
        videobuff[2*i + 1] = OUTPUT_COLOR;
    }
    CURR_LINE_SYMB = LINE_START;
    cursor_moveto(CURR_CONSOLE_LINE, 0);
}

void clear_console()
{
    char * vb = (char*)0xb8000;
    for (int i = 0; i < CONSOLE_HEIGHT * CONSOLE_WIDTH; i++)
    {
        vb[2*i] = ' ';
        vb[2*i+1] = OUTPUT_COLOR;
    }
    CURR_CONSOLE_LINE = -1;
    next_line();
}

void put_symb(char symb) 
{
    if (symb == '\n') 
    {
        next_line();
        return;
    }
    unsigned char* videobuff = (unsigned char*) VIDEO_BUF_PTR;
    videobuff += CURR_LINE_SYMB * 2;
    videobuff[0] = symb;
    videobuff[1] = OUTPUT_COLOR;

    CURR_LINE_SYMB++;

    if (CURR_LINE_SYMB > CONSOLE_WIDTH) next_line();

    cursor_moveto(CURR_CONSOLE_LINE, CURR_LINE_SYMB);
}

void out_str(const char* ptr)
{
    while (*ptr)
    {
        put_symb(*ptr++);
    }
}

void putline(const char* line)
{
    out_str(line);
    put_symb('\n');
}

void getline(char *buff, int size) 
{
    while (scaning);
    scaning = 1;
    NEXT_SYMB = 0;
    while (scaning);
    memcpy(buff, GLOB_BUFF, MIN(size, NEXT_SYMB));
    buff[size - 1] = '\0';
}
