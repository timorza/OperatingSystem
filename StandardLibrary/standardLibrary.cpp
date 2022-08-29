#include "standardLibrary.h"
#include "../Kernel/Panic.h"
#include "../Drivers/screenDriver.h"

void swap(char *x, char *y) 
{
    char t = *x;
    *x = *y;
    *y = t;
}
 
char* reverse(char *buffer, int i, int j)
{
    while (i < j) 
    {
        swap(&buffer[i++], &buffer[j--]);
    }
 
    return buffer;
}

int strlen(const char *s)
{
    int count = 0;

    while(*s != '\0')
    {
        count++;
        s++;
    }

    return count;
}

void append(char s[], char n)
{
    int len = strlen(s);
    s[len] = n;
    s[len+1] = '\0';
}
 
int atoi(const char* str)
{
    int res = 0;
    int sign = 1;
    int i = 0;
 
    if (str[0] == '-')
    {
        sign = -1;
        i++;
    }

    for (i = 0; str[i] != '\0'; i++)
    {
        res = res * 10 + str[i] - '0';
    }
 
    return sign * res;
}

char* itoa(int value, char* buffer, int base)
{
    if (base < 2 || base > 32) 
    {
        return buffer;
    }

    int n = 0;
    int i = 0;

    if (value < 0)
    {
        n = value * (-1);
    }
    else
    {
        n = value;
    }

    while (n)
    {
        int r = n % base;
 
        if (r >= 10) 
        {
            buffer[i++] = 65 + (r - 10);
        }
        else 
        {
            buffer[i++] = 48 + r;
        }
 
        n = n / base;
    }
 
    if (i == 0) 
    {
        buffer[i++] = '0';
    }
 
    if (value < 0 && base == 10) 
    {
        buffer[i++] = '-';
    }
 
    buffer[i] = '\0'; 

    return reverse(buffer, 0, i - 1);
}

char* strcpy(char* destination, const char* source)
{
    if(destination == nullptr)
    {
        return nullptr;
    }

    char *ptr = destination;

    while (*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }
 
    *destination = '\0';
 
    return ptr;
}

int strcmp(char *x, char *y)
{
    int i = 0;
    int failed = 0;
    while(x[i] != '\0' && y[i] != '\0')
    {
        if(x[i] != y[i])
        {
          failed = 1;
          break;
        }
        i++;
    }
    if( (x[i] == '\0' && y[i] != '\0') || (x[i] != '\0' && y[i] == '\0') )
       failed = 1;
  
      return failed;
}
 
void memset(uint8_t* dest, unsigned char val, uint32_t len)
{
    for (uint8_t* temp = dest; len != 0; len--)
    {
        *temp++ = val;
    } 
}

void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len)
{
    const uint8_t *sp = (const uint8_t *)src;
    uint8_t *dp = (uint8_t *)dest;
    for(; len != 0; len--) *dp++ = *sp++;
}

void hex_to_ascii(int n, char str[])
{
    append(str, '0');
    append(str, 'x');
    char zeros = 0;

    uint32_t tmp;
    int i = 0;
    for (i = 28; i > 0; i -= 4)
    {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0)
        {
            continue;
        }
        zeros = 1;
        if (tmp > 0xA)
        {
            append(str, tmp - 0xA + 'a');
        }
        else
        {
            append(str, tmp + '0');
        }
    }

    tmp = n & 0xF;

    if (tmp >= 0xA)
    {
        append(str, tmp - 0xA + 'a');
    }
    else
    {
        append(str, tmp + '0');
    }
}
