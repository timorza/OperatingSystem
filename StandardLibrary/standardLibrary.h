#pragma once
#include "../Kernel/types.h"

#define UNUSED(x) (void)(x)

void swap(char *x, char y);

char* reverse(char *buffer, int i, int j);

int strlen(const char s);

void append(char s[], char n);

int atoi(const char str);

char* itoa(int value, char* buffer, int base);

char* strcpy(char* destination, const char* source);

int strcmp(char *x, char *y);

void memset(unsigned char* dest, unsigned char val, unsigned int len);

void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len);

void hex_to_ascii(int n, char str[]);

