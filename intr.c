#include "intr.h"

struct idt_entry g_idt[256]; // Реальная таблица IDT
struct idt_ptr g_idtp; 


void default_intr_handler()
{
    asm("pusha");
        // ... (реализация обработки)
    asm(
        "popa"      "\n\t"
        "leave"     "\n\t"
        "iret"      "\n\t"
        );
}

void intr_reg_handler(int num, unsigned short segm_sel, unsigned short
flags, intr_handler hndlr)
{
    unsigned int hndlr_addr = (unsigned int) hndlr;
    g_idt[num].base_lo = (unsigned short) (hndlr_addr & 0xFFFF);
    g_idt[num].segm_sel = segm_sel;
    g_idt[num].always0 = 0;
    g_idt[num].flags = flags;
    g_idt[num].base_hi = (unsigned short) (hndlr_addr >> 16);
}
// Функция инициализации системы прерываний: заполнение массива с адресами обработчиков
void intr_init()
{
    int i;
    int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);
    for(i = 0; i < idt_count; i++)
        intr_reg_handler(  
                        i, 
                        GDT_CS,
                        0x80 | IDT_TYPE_INTR,
                        default_intr_handler); // segm_sel=0x8, P=1, DPL=0, Type=Intr
}

void intr_start()
{
    int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);
    g_idtp.base = (unsigned int) (&g_idt[0]);
    g_idtp.limit = (sizeof (struct idt_entry) * idt_count) - 1;
    asm("lidt %0" : : "m" (g_idtp) );
}

void intr_enable()
{
    asm("sti");
}

void intr_disable()
{
    asm("cli");
}