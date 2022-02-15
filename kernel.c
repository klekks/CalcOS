__asm("jmp kmain");

#include "intr.h"
#include "keyboard.h"
#include "utils.h"

const char* g_test = "This is test string.";

enum type_{num, op};

struct queue_elem
{
    enum type_ type;
    int value;
};

char* read_token(char *str)
{
    while (*str && isspace(*str)) str++;

    if (!*str) return str;

    if (isdigit(*str))
        while (*str && isdigit(*str)) str++;
    else 
        str++;

    return str;
}

int check_expr(char *str)
{
    char q[64];
    int head = 0;

    while (*str) 
    {
        char * token = str;
        str = read_token(str);
            //if minus       and it is the first symb or last symb is operator
        if (*token == '-' && (!head || q[head - 1] == 'o')) q[head++] = '-';
        else if (*token == '-' && q[head - 1] == '-') return 0;
        else if (isdigit(*token)) q[head++] = 'n';
        else if (isoperator(*token))
        {
            if (q[head - 1] == 'o') return 0;
            q[head++] = 'o';
        }
        else return 0;
    }
    return 1;
}

void exec(char *inp)
{
    char op_stack[32] = {0};
    struct queue_elem shunting_queue[64];
    int stack_head = 0, queue_head = 0;
    int prev_is_num = 0;
    while (*inp)
    {
        char *token = inp;
        inp = read_token(inp); // [token, input) -- body of token
        if (!prev_is_num && *token == '-') inp = read_token(inp);
        if (isoperator(*token) && !(!prev_is_num && *token == '-'))
        {
            prev_is_num = 0;
            if (*token == '-' || *token == '+')
            {
                while (stack_head)
                {
                    shunting_queue[queue_head].type = op;
                    shunting_queue[queue_head].value = op_stack[stack_head - 1];
                    queue_head++;
                    stack_head--;
                }
                op_stack[stack_head] = *token;
                stack_head++;
            }
            else
            {
                while (stack_head && (op_stack[stack_head - 1] == '*' || op_stack[stack_head - 1] == '/'))
                {
                    shunting_queue[queue_head].type = op;
                    shunting_queue[queue_head].value = op_stack[stack_head - 1];
                    queue_head++;
                    stack_head--;
                }
                op_stack[stack_head] = *token;
                stack_head++;
            }
        }
        else 
        {
            prev_is_num = 1;
            shunting_queue[queue_head].type = num;
            shunting_queue[queue_head].value = atoi(token);
            queue_head++;
        }
    }

    while (stack_head)
    {
        shunting_queue[queue_head].type = op;
        shunting_queue[queue_head].value = op_stack[stack_head - 1];
        queue_head++;
        stack_head--;
    }

    int exec_stack[64];
    int exec_head = 0;

    for (int q = 0; q < queue_head; q++)
    {
        if (shunting_queue[q].type == num) 
        {
            exec_stack[exec_head++] = shunting_queue[q].value;
        }
        else
        {
            int op2 = exec_stack[exec_head-1],
                op1 = exec_stack[exec_head-2];
            exec_head--; exec_head--;
            int res = 0;
            switch(shunting_queue[q].value) 
            {
                case '-':
                    res = op1 - op2;
                    break;
                case '+':
                    res = op1 + op2;
                    break;
                case '/':
                    if (op2 == 0)
                    {
                        putline("Zero division.");
                        return;
                    }
                    res = op1 / op2;
                    break;
                case '*':
                    res = op1*op2;
                    break;
            }
            exec_stack[exec_head++] = res;
        }

    }
    char buff[18];
    itoa(exec_stack[0], buff);
    putline(buff);
}

void exit() {
    outw(0xB004, 0x2000);
    outw(0x604, 0x2000);
    outw(0x4004, 0x3400);
}

extern "C" int kmain()
{
    const char* hello = "Welcome to HelloWorldOS (gcc edition)!";

    intr_init();
    keyb_init();
    intr_start();
    intr_enable();
    clear_console();

    putline("Welcome to CALCOS");

    char bf[256];
    while (1) 
    {
        out_str(">>> ");
        getline(bf, 256);
        if (!strcmp(bf, "shutdown")) exit();
        else if (!strcmp(bf, "info"))
            putline("CalcOS by Ilya Petrov. SPBSTU 2022, 4831001/00001");
        else
        {
            if (check_expr(bf))
                exec(bf);
            else 
                putline("Incorrect expretion.");
        }
    }

    while(1) asm("hlt");

    return 0;
}
