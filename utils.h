#pragma once
int strlen(char * str);
void reverse(char s[]);
void itoa(int n, char s[]);
int isspace(char c);
int isdigit(char c);
int isoperator(char c);
int atoi (char *s);

int strcmp(const char * s1, const char * s2);
void memcpy(void * dst, const void * src, int cnt);
#define MIN(A, B) (A < B ? A : B)
#define MAX(A, B) (A > B ? A : B)