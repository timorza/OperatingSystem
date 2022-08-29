//Files that will be used
#include "screenDriver.h"
#include "../Hardware/IO.h"

//Declaring private functions only accssible by file
int print_char(char c, int col, int row, char color);
int get_cursor_offset();
void set_cursor_offset(int offset);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);
void resetingCursor();



//PUBLIC FUNCTIONS

/**
 * Print a message in the specified location
 * If col, row, are negative, print in the current offset
 */
void printk_at(char *message, int col, int row, char attributes) {
    //Setting the cursor
    int offset;
    if (col >= 0 && row >= 0)
        offset = get_offset(col, row);
    else {
        offset = get_cursor_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    // Loop through message and print it 
    for(int i = 0; message[i] != 0; i++)
    {
        offset = print_char(message[i],col,row,attributes);
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    
}

char get_char_attributes(int isBlink,int background, int foreground)
{
    char res = 0;
    //MSB is responsible for blinking, bit 7
    if (isBlink == 1) res = res | (1 << 8);

    //Background, 3 bits before the blink, bits 4-6
    if(background < 8 || background < 0) res = res | (background << 4);

    //Foreground, 4 bits before the background, bits 0-3
    if(foreground < 18 || foreground < 0) res = res | foreground;
    else res = res | WHITE;
    return res;
}


//aka famous kernel print, with color!
void printk(char* message, char attributes) 
{    
    printk_at(message, -1, -1,attributes);
}

//White on black print
void printk(char* message)
{
    printk_at(message,-1,-1,get_char_attributes(0,BLACK,WHITE));
}


//PRIVATE FUNCTIONS - DRIVER ONLY
/*
    Most basic print function of the kernel,
    acsess the memory directly and write the characters and also
    dealing with offsets and 2d screen
*/
int print_char(char chr , int col, int row, char color) {
    unsigned char* video_memory = (unsigned char*) VIDEO_ADDRESS; //The video address,the place in the memory for VGA

    //If cords are problematic, Display an error at bottom of screen, while we don't have any interrupts
    if (row >= MAX_ROWS || col >= MAX_COLS) {
        video_memory[2*(MAX_COLS)*(MAX_ROWS)-4] = 'E';
        video_memory[2*(MAX_COLS)*(MAX_ROWS)-3] = get_char_attributes(1,RED,BLACK);

        video_memory[2*(MAX_COLS)*(MAX_ROWS)-2] = 'R';
        video_memory[2*(MAX_COLS)*(MAX_ROWS)-1] = get_char_attributes(1,RED,BLACK);
        return get_offset(col, row);
    }

    //Getting the offset
    int offset;
    if (col >= 0 && row >= 0) 
    	offset = get_offset(col, row);
    else
    	offset = get_cursor_offset();


    if (chr == '\n') //Increase the number of rows by one in offset
    {
        row = get_offset_row(offset);
        offset = get_offset(0, row+1);
    } 
    else 
    {
        video_memory[offset] = chr; //First byte is the char we want to output
        
        //If we don't have any requests for color, print default white on black
        if (!color) color = WHITE_ON_BLACK;
        video_memory[offset+1] = color;//Second byte is the attributes
        offset += 2;
    }
    
    //SCROLL SCREEN IF NEEDED CHECK
    //Check if we need to scroll the screen down(if we are at the bottom most row)
    if(offset >= MAX_COLS * MAX_ROWS * 2)//LAST 
    {
        for(int i = 1; i < MAX_ROWS; i++)//Advancing each row backwards, instead of the first
        {
            //Copying each memory cell to the next row
            for(int f = 0; f< MAX_COLS * 2; f++)
                video_memory[get_offset(f,i -1)] = video_memory[get_offset(f,i)];
        }

        //Leaving the last line blank
        char* last_line = get_offset(0,MAX_COLS - 1) + VIDEO_ADDRESS;
        for(int i = 0; i < MAX_COLS * 2; i++)
            last_line[i] == 0;

        offset -= 2 * MAX_COLS;
    }


    set_cursor_offset(offset);
    return offset;
}




int get_cursor_offset() {
    /* Use the VGA ports to get the current cursor position
     * 1. Ask for high byte of the cursor offset (data 14)
     * 2. Ask for low byte (data 15)
     */
    outb(REG_SCREEN_CTRL, 14);
    int offset = inb(REG_SCREEN_DATA) << 8; // High byte: << 8 
    outb(REG_SCREEN_CTRL, 15);
    offset += inb(REG_SCREEN_DATA);
    return offset * 2; // Position * size of character cell 
}

void set_cursor_offset(int offset) {
    // Similar to get_cursor_offset, but instead of reading we write data
    offset /= 2;
    outb(REG_SCREEN_CTRL, 14);
    outb(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    outb(REG_SCREEN_CTRL, 15);
    outb(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

void clear_screen() {
    int screen_size = MAX_COLS * MAX_ROWS;
    char *screen = VIDEO_ADDRESS;

    for (int i = 0; i < screen_size; i++) {
        screen[i*2] = ' ';
        screen[i*2+1] = WHITE_ON_BLACK;
    }

    resetingCursor();
}

void resetingCursor()
{
    set_cursor_offset(get_offset(0, 0));	
}

int get_offset(int col, int row) 
{ 
	return 2 * (row * MAX_COLS + col); 
}

int get_offset_row(int offset) 
{ 
	return offset / (2 * MAX_COLS);
}


int get_offset_col(int offset) 
{
	return (offset - (get_offset_row(offset)*2*MAX_COLS))/2; 
}