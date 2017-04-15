#include<unistd.h>
#include<termios.h>
#include<stdlib.h>
#include<ctype.h>
#include<stdio.h>

struct termios org_termios;

void disableRawMode(){
	//TCSAFLUSH flushes any input before setting the bitflags.
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &org_termios);
}

void enableRawMode(){
	tcgetattr(STDIN_FILENO, &org_termios);
	struct termios raw = org_termios;
	
	//registering the disableRawMode() function to run when the program exits.
	atexit(disableRawMode);
	//ICANON bitflag sets the input in canonical mode.
	raw.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main(){
	char c;
	enableRawMode();
	//read and STDIN_FILENO is from unistd.h
	//read() reads one character from stdin and stores it to c.
	//read() reads until it reaches the EOF. This is signalled ny ^-D.
	while (read(STDIN_FILENO, &c, 1) == 1 && c!='q'){
		if(iscntrl(c)){
			printf("%d\n",c);
		}
		else{
			printf("%d (%c)\n",c,c);
		}
	}

	return 0;
}