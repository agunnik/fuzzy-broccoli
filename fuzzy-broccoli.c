#include<unistd.h> //read()
#include<termios.h> //for all the terminal bit flags to enable raw mode.
#include<stdlib.h>	//exit() ??
#include<errno.h>	//perror()
#include<ctype.h>
#include<stdio.h>

#define CTRL_KEY(k) ((k) & 0x1f)
struct termios org_termios;

void die(const char *s){
	perror(s);
	exit(1);
}

//restores canonical mode.
void disableRawMode(){
	//TCSAFLUSH flushes any input before setting the bitflags.
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &org_termios) == -1) 
		die("tcsetattr");
}

void enableRawMode(){
	if(tcgetattr(STDIN_FILENO, &org_termios) == -1)
		die("tcgetattr");
	struct termios raw = org_termios;
	
	//registering the disableRawMode() function to run when the program exits. atexit() is from stdlib.h.
	atexit(disableRawMode);

	//ICANON bitflag sets the input in canonical mode.
	//ISIG enables sigs INTR, QUIT & SUSP
	//ECHO enables echoing.
	//IEXTEN enables Ctrl-V and (only on macOS) Ctrl-O.

	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

	//IXON is an input flag. It disables input control flow. Ctrl-s and Ctrl-Q can now be detected.
	//fixes Ctrl-m with ICRNL.
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	//disabling output formatting.
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);

	//setting timeout for read()
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die("tcsetattr");
}

int main(){
	char c;
	enableRawMode();
	//read and STDIN_FILENO is from unistd.h
	//read() reads one character from stdin and stores it to c.
	//read() reads until it reaches the EOF. This is signalled ny ^-D.
	while (1){
		c ='\0';
		if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
			die("read");
		if(c == CTRL_KEY('q')){
			printf("EXIT\r\n");
			break;
		}
		if(iscntrl(c)){
			printf("%d\r\n",c);
		}
		else{
			printf("%d (%c)\r\n",c,c);
		}
	}

	return 0;
}