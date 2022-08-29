#include "keyboard.h"
#include "../Hardware/IO.h"
#include "../Interrupts/isr.h"
#include "screenDriver.h"
#include "../StandardLibrary/standardLibrary.h"
#define SC_MAX 57
#define BACKSPACE 0x0E
#define ENTER 0x1C

static char key_buffer[256];

const char* pictures[4] = {"   _\n  \\`\"-.\n   )  _`-.\n  ,  : `. \\\n  : _   '  \\\n  ; *` _.   `--._\n  `-.-'          `-.\n    |       `       `.\n    :.       .        \\\n    | \\  .   :   .-'   .\n    :  )-.;  ;  /      :\n    :  ;  | :  :       ;-.\n    ; /   : |`-:     _ `- )\n ,-' /  ,-' ; .-`- .' `--'\n `--'   `---' `---'" , "     .\n    / \\\n    | |\n    |.|\n    |.|\n    |:|      __\n  ,_|:|_,   /  )\n    (Oo    / _I_\n     +\\ \\  || __|\n        \\ \\||___|\n          \\ /.:.\\-\\\n           |.:. /-----\\\n           |___|::oOo::|\n           /   |:<_T_>:|\n          |_____\\ ::: /\n           | |  \\ \\:/\n           | |   | |\n           \\ /   | \\\n           / |   \\____\\\n           `-'" ,"       ____\n    .-\" +' \"-.\n   /.'.'A_'*`.\\\n  |:.*'/\\-\\. ':|\n  |:.'.||\"|.'*:|\n   \\:~^~^~^~^:/\n    /`-....-'\\\n   /          \\\n   `-.,____,.-'", "########:::::::::::########\n#########:::::::::#########\n##########:::::::##########\n###########,---.###########\n##########/`---'\\##########\n#########/       \\#########\n########/         \\########\n#######:`-._____.-':#######\n######::::: ( ) |::::######\n#####:::::: ) ( o:::::#####\n####::::: .-(_)-. :::::####\n###:::::: '=====' ::::::###" };
uint8_t currentPicture = 0;
uint8_t isViewingPictures = 0;

const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6", 
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", 
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", 
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", 
        "/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
        'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G', 
        'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V', 
        'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '};

static void keyboard_callback(registers_t regs) {
    // The PIC leaves the scancode in port 0x60
    uint8_t scancode = inb(0x60);
    
    
    if (scancode > SC_MAX)
    {
        return;
    }
    else if(isViewingPictures == 1)
    {
        clear_screen();
        char letter = sc_ascii[(int)scancode];
        char str[2] = {letter, '\0'};
        append(key_buffer, letter);
        checkCommands();
    }
    else if (scancode == ENTER)
    {
        printk(STR"\n");
        checkCommands();
    }
    else 
    {
        char letter = sc_ascii[(int)scancode];

        char str[2] = {letter, '\0'};
        append(key_buffer, letter);
        printk(str, get_char_attributes(0,BLACK,WHITE));
    }
    UNUSED(&regs);
}


void  printPictures(uint8_t currentPicture)
{
    if(currentPicture == 0)
        printk(STR"  _\n  \\`\"-.\n   )  _`-.\n  ,  : `. \\\n  : _   '  \\\n  ; *` _.   `--._\n  `-.-'          `-.\n    |       `       `.\n    :.       .        \\\n    | \\  .   :   .-'   .\n    :  )-.;  ;  /      :\n    :  ;  | :  :       ;-.\n    ; /   : |`-:     _ `- )\n ,-' /  ,-' ; .-`- .' `--'\n `--'   `---' `---'");
    else if(currentPicture == 1)
        printk(STR"     .\n    / \\\n    | |\n    |.|\n    |.|\n    |:|      __\n  ,_|:|_,   /  )\n    (Oo    / _I_\n     +\\ \\  || __|\n        \\ \\||___|\n          \\ /.:.\\-\\\n           |.:. /-----\\\n           |___|::oOo::|\n           /   |:<_T_>:|\n          |_____\\ ::: /\n           | |  \\ \\:/\n           | |   | |\n           \\ /   | \\\n           / |   \\____\\\n           `-'");
    else if(currentPicture == 2)
        printk(STR"       ____\n    .-\" +' \"-.\n   /.'.'A_'*`.\\\n  |:.*'/\\-\\. ':|\n  |:.'.||\"|.'*:|\n   \\:~^~^~^~^:/\n    /`-....-'\\\n   /          \\\n   `-.,____,.-'");
    else if(currentPicture == 3)
        printk(STR"########:::::::::::########\n#########:::::::::#########\n##########:::::::##########\n###########,---.###########\n##########/`---'\\##########\n#########/       \\#########\n########/         \\########\n#######:`-._____.-':#######\n######::::: ( ) |::::######\n#####:::::: ) ( o:::::#####\n####::::: .-(_)-. :::::####\n###:::::: '=====' ::::::###");
}

void clearBuffer()
{
    for(int i = 0; i < 256; i++)
        key_buffer[i] = 0;
}

void checkCommands()
{
    if(isViewingPictures == 1)
    {
        clear_screen();
        printk(STR"Enter '1' for the next picture and '2' to exit pictures\n");
        if(key_buffer[0] == '1')//Next picture
        {
            printPictures(currentPicture);
            currentPicture = (currentPicture + 1) % 4;
        }
        else if(key_buffer[0] == '2')//Exit
        {
            isViewingPictures = 0;
            clear_screen();
	        printk(STR"Enter '1' for further information\n");
        }
        else
            printPictures(currentPicture);
    }
    else if(key_buffer[0] == '1')//Help
    {
        printk(STR"List of commands(Enter numbers):\n");
        printk(STR"1. help - This message\n");
        printk(STR"2. info - Showing information about the operating system\n");
        printk(STR"3. pictures - Show some lovely ASCII pictures\n");
        printk(STR"4. clear - Clears the screen\n");
    }
    else if(key_buffer[0] == '2')//Info
    {
        printk(STR"This is an 32bit operating system for the x86 computer architechture, created by Michael Klotsky and Timor Zalesky\n");
        printk(STR"--This operating system includes:\n");
        printk(STR"--Full Interrupt Support\n");
        printk(STR"--Keyboard driver - a p2 keyboard that includes most of the keys\n");
        printk(STR"--Screen driver - a VGA driver\n");
        printk(STR"--PIT Driver\n");
        printk(STR"--Paging Enabled\n");
    }
    else if(key_buffer[0] == '3')//Pictures
    {
        clear_screen();
        isViewingPictures = 1;
        printk(STR"Enter '1' for the next picture and '2' to exit pictures\n");

    }
    else if(key_buffer[0] == '4')
        clear_screen();
    else
        printk(STR"ERROR - Uknown Command\n");
    clearBuffer();
}

void init_keyboard() 
{
    register_interrupt_handler(IRQ1, keyboard_callback); 
    printk(STR"[OK] ", get_char_attributes(0,BLACK,GREEN));
    printk(STR"Keyboard succesfully set\n");
}
