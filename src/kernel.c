#include <stdint.h>

// DEFINES
#define bool unsigned short
#define true 1
#define false 0

#define videoAddress 0xB8000

// COLOR CONSTANTS
const int COLOR_BLACK = 0x00;
const int COLOR_BLUE = 0x01;
const int COLOR_GREEN = 0x02;
const int COLOR_CYAN = 0x03;
const int COLOR_RED = 0x04;
const int COLOR_MAGENTA = 0x05;
const int COLOR_BROWN = 0x06;
const int COLOR_GRAY = 0x07;
const int COLOR_DARK_GRAY = 0x08;
const int COLOR_LIGHT_BLUE = 0x09;
const int COLOR_LIGHT_GREEN = 0x0A;
const int COLOR_LIGHT_CYAN = 0x0B;
const int COLOR_LIGHT_RED = 0x0C;
const int COLOR_LIGHT_MAGENTA = 0x0D;
const int COLOR_YELLOW = 0x0E;
const int COLOR_WHITE = 0x0F;

// VARIABLES
const char keys[] =	{
					0,		// 1
					0,		// 2
					0,		// ..
					0,
					0,
					0,
					0,
					0,
					0,
					0,
					0,
					0,
					0,
					0,
					0,		// BACKSPACE
					0,		// 15
					'q',	// 16
					'w',	// 17
					'e',	// 18
					'r',	// 19
					't',	// 20
					'y',	// 21
					'u',	// 22
					'i',	// 23
					'o',	// 24
					'p',	// 25
					0,		// 26
					0,		// 27
					0,		// ENTER
					0,		// 29
					'a',	// 30
					's',	// 31
					'd',	// 32
					'f',	// 33
					'g',	// 34
					'h',	// 35
					'j',	// 36
					'k',	// 37
					'l',	// 38
					0,		// 39
					0,		// 40
					0,		// ..
					0,		
					0,
					'z',	// 44
					'x',	// 45
					'c',	// 46
					'v',	// 47
					'b',	// 48
					'n',	// 49
					'm',	// 50
					};

const int LINE_LENGTH = 80;
const int LINE_ROWS = 25;
const int COLOR_DEFAULT = 0x07;
#define BUFFER_SIZE 200

volatile unsigned char *video = (unsigned char *)videoAddress;
unsigned long long ticks = 0;
int row = 0;
int column = 0;
unsigned int scancode = 100;
unsigned int dscancode = 100;
char cmd[BUFFER_SIZE] = {0};
unsigned int cmdCount = 0;
unsigned int clears = 0;

// METHODS
inline void outb( unsigned short port, unsigned char val );
inline unsigned char inb( unsigned short port );
void print(char *str);
void print_col(char *str, int col);
void printchar(char c);
void println(char *str);
void println_col(char *str, int col);
void printSingleAt(unsigned char c, unsigned char forecol, unsigned char backcol, int x, int y);
void update_cursor(int x, int y);
void clear_screen();
void newln();
void sleep(long ticks);
char *istr(int i);
int clen(char *str);
bool cequ(char str1[], char str2[]);
char *cadd(char *str1, char *str2);

inline void outb( unsigned short port, unsigned char val ) {
    asm volatile( "outb %0, %1"
                  : : "a"(val), "Nd"(port) );
}

inline unsigned char inb( unsigned short port ) {
    unsigned char ret;
    asm volatile( "inb %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}

void kmain(void) {
	
	clear_screen();
	println_col("Starting Kernel ..", COLOR_LIGHT_GREEN);

	extern uint32_t magic;
	
	if (magic != 0x2BADB002) {
		println_col("ERROR: Failed to start Kernel!", COLOR_RED);
		return;
	}
	
	println_col("#############################################",COLOR_LIGHT_CYAN);
	sleep(2000000);
	println_col("#                                           #",COLOR_LIGHT_CYAN);
	sleep(2000000);
	println_col("#                                           #",COLOR_LIGHT_CYAN);
	sleep(2000000);
	print_col("#                 ",COLOR_LIGHT_CYAN);
	print_col("TypexOS",COLOR_CYAN);
	print_col("                   #",COLOR_LIGHT_CYAN);
	newln();
	sleep(2000000);
	print_col("#                  ",COLOR_LIGHT_CYAN);
	print_col("v 0.1",COLOR_CYAN);
	print_col("                    #",COLOR_LIGHT_CYAN);
	newln();
	sleep(2000000);
	println_col("#                                           #",COLOR_LIGHT_CYAN);
	println_col("#                                           #",COLOR_LIGHT_CYAN);
	println_col("#############################################",COLOR_LIGHT_CYAN);
	sleep(2000000);

	println_col("Kernel Started!", COLOR_GREEN);
	print_col("Type ", COLOR_LIGHT_RED);
	print_col("help",COLOR_BROWN);
	print_col(" for a list of commands.",COLOR_LIGHT_RED);
	println("");
	print("> ");

	while(true) {
		scancode = inb(0x60);
		if(scancode < 100 && scancode != dscancode) {
			if(scancode == 28) { // ENTER
				if(cequ(cmd,"")) {
					continue;
				}
				println("");
				print_col(">> ", COLOR_YELLOW);
				print_col(cmd,COLOR_LIGHT_BLUE);
				println("");
				if(cequ(cmd,"exit")) {
					println_col("Exiting ..", COLOR_RED);
					return;
				} else if(cequ(cmd,"help")) {
					println_col("List of commands:", COLOR_LIGHT_RED);
					println_col("> help : Show this help", COLOR_YELLOW);
					println_col("> exit : Exit the Kernel", COLOR_YELLOW);
					println_col("> cls  : Clear the Screen", COLOR_YELLOW);
				} else if (cequ(cmd, "cls")) {
					clear_screen();
				} else {
					println_col(cadd("Unknown command: ",cmd),COLOR_RED);
					print_col("Type ", COLOR_LIGHT_RED);
					print_col("help",COLOR_BROWN);
					print_col(" for a list of commands.",COLOR_LIGHT_RED);
					println("");
				}
				cmdCount = 0;
				int i = 0;
				while(i < BUFFER_SIZE) {
					cmd[i] = 0;
					i++;
				}
				print("> ");
			} else if(scancode == 57) { // SPACE
				print(" ");
				cmd[cmdCount] = ' ';
				cmdCount++;
			} else if(scancode == 14) { // BACKSPACE
				if(cmdCount != 0) {
					cmd[cmdCount-1] = 0;
					cmdCount--;
					column--;
					printSingleAt(' ', COLOR_BLACK, COLOR_BLACK, column, row);
				}
			} else {
				char c[80];
				c[0] = keys[scancode];
				c[1] = '\0';
				// add to command
				cmd[cmdCount] = keys[scancode];
				cmdCount++;
				print(c);
			}
		}
		dscancode = scancode;
		ticks++;
	}

}

void print(char *str) {

	int i = 0;
    while( str[i] != 0 ) {
		printSingleAt(str[i], COLOR_DEFAULT, COLOR_BLACK, column, row);
		i++;
		column++;
    }
}

void print_col(char *str, int col ) {

	int i = 0;
    while( str[i] != 0 ) {
		printSingleAt(str[i], col, COLOR_BLACK, column, row);
		i++;
		column++;
    }
}

void printchar(char c) {
	printSingleAt(c, COLOR_DEFAULT, COLOR_BLACK, column, row);
}

void println(char *str) {
	print(str);
	newln();
}

void println_col(char *str, int col ) {
	print_col(str, col);
	newln();
}

void printSingleAt(unsigned char c, unsigned char forecolour, unsigned char backcolour, int x, int y) {
     uint16_t attrib = (backcolour << 4) | (forecolour & 0x0F);
     volatile uint16_t * where;
     where = (volatile uint16_t *)videoAddress + (y * LINE_LENGTH + x) ;
     *where = c | (attrib << 8);

	 update_cursor(x,y);

	 if(row > LINE_ROWS) {
		 clear_screen();
	 }
}

void update_cursor(int x, int y) {

	unsigned short position = y*LINE_LENGTH + x;

	// cursor HIGH port to vga INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((position>>8)&0xFF));
	// cursor LOW port to vga INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));
}

void clear_screen() {
	volatile unsigned char *video = (unsigned char *)videoAddress;
	int i = 0;
	while(i < LINE_ROWS * LINE_LENGTH * 2) {
		video[i] = ' ';
		video[i+1] = COLOR_BLACK;
		i += 2;
	}
	row = 0;
	column = 0;

	clears++;
}

void newln() {
	column = 0;
	row++;
}

void sleep(long ticks) {

	long i = 0;

	while((volatile long)ticks > i) {
		i++;
	}

}

char *istr(int i) {
	int num = i;
	char *res = "";
	int len = 0;
	while( num > 0) {
		res[len] = num%10+'0';
		num/=10;
		++len;
	}
	res[len] = 0;
	int n = 0;
	while( n < len/2 ) {
		char c = res[n];
		res[n] = res[len-n-1];
		res[len-n-1] = c;
		++n;
	}
	return res;
}

int clen(char *str) {
	int len = 0;
	while(str[len] != 0) {
		len++;
	}
	return len;
}

bool cequ(char str1[], char str2[]) {

	bool ret = true;

	if(clen(str1) != clen(str2)) {
		return false;
	}

	int i = 0;
	while(str1[i] != 0) {
		if(str1[i] != str2[i]) {
			ret = false;
			break;
		} else {
			ret = true;
		}
		i++;
	}

	return ret;

}

char *cadd(char *str1, char *str2) {

	char ret[BUFFER_SIZE] = "";

	int i = 0;
	while(str1[i] != 0) {
		ret[i] = str1[i];
		i++;
	}

	int len = i;

	i = 0;
	while(str2[i] != 0) {
		ret[len+i] = str2[i];
		i++;
	}
	
	char* ret1 = ret;
	return ret1;
}
