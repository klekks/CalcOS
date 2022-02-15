int strlen(char * str) {
    int len;
    for (len = 0; *str; ++str, len++);
    return len;
}

void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

 void itoa(int n, char s[])
 {
     int i, sign;
 
     if ((sign = n) < 0)  /* записываем знак */
         n = -n;          /* делаем n положительным числом */
     i = 0;
     do {       /* генерируем цифры в обратном порядке */
         s[i++] = n % 10 + '0';   /* берем следующую цифру */
     } while ((n /= 10) > 0);     /* удаляем */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

int isspace(char c) 
{
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

int isdigit(char c) 
{
    return c >= '0' && c <= '9';
}

int atoi (char *s) 
{
    int z = 0, n = 0;

    while (isspace(*s)) s++;

    if (*s == '-') 
    { 
        z = 1; 
        s++; 
    } 
    else if (*s == '+') s++;

    while (isdigit(*s)) n = 10 * n + *s++ - '0';
    return ( z ? -n : n);
}

void memcpy(void * dst, const void * src, int cnt)
{
    unsigned char * d = (unsigned char*)dst;
    const unsigned char * s = (const unsigned char*)src;
    
    while (cnt--) *d++ = *s++;
    
    return ;
}

int isoperator(char c)
{
    return c == '-' || c == '+' || c == '*' || c == '/';
}

int strcmp(const char * s1, const char * s2)
{
    while (*s1 == *s2 && *s1++ | *s2++);
    int i = *s1 - *s2;
    return i < 0 ? *s1 : i > 0 ? *s2 : 0;
}