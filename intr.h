#pragma once

#define IDT_TYPE_INTR (0x0E)
#define IDT_TYPE_TRAP (0x0F)

#define GDT_CS (0x8)
// Структура описывает данные об обработчике прерывания
struct idt_entry
{
 unsigned short base_lo; // Младшие биты адреса обработчика
 unsigned short segm_sel; // Селектор сегмента кода
 unsigned char always0; // Этот байт всегда 0
 unsigned char flags; // Флаги тип. Флаги: P, DPL, Типы - это константы - IDT_TYPE...
 unsigned short base_hi; // Старшие биты адреса обработчика
} __attribute__((packed)); // Выравнивание запрещено

// Структура, адрес которой передается как аргумент команды lidt
struct idt_ptr
{
 unsigned short limit;
 unsigned int base;
} __attribute__((packed)); // Выравнивание запрещено

void intr_enable();
void intr_disable();
void intr_init();
void intr_start();
typedef void (*intr_handler)();
void intr_reg_handler(int num, unsigned short segm_sel, unsigned short flags, intr_handler hndlr);