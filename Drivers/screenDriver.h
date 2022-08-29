#pragma once
#define VIDEO_ADDRESS (char*)0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

//Colors
#define WHITE_ON_BLACK 0x0f
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHT_GRAY 7
#define DARK_GRAY 8
#define LIGHT_BLUE 9
#define LIGHT_GREEN 10
#define LIGHT_CYAN 11
#define LIGHT_RED 12
#define PINK 13
#define YELLOW 14
#define WHITE 15


// Screen i/o ports 
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

// Public available kernel API 
void clear_screen();
void printk_at(char *message, int col, int row);
void printk(char* message);
void printk(char* message, char attributes);
char get_char_attributes(int isBlink,int background, int foreground);