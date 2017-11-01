#include<termios.h>
#include<unistd.h>
#include<printf.h>
#include<errno.h>
#include<stdlib.h>

struct termios org_termios;

void enableRawMode(){
	if(tcgetattr(STDIN_FILENO, &org_termios) == -1){
		perror("Died during tcgetattr().");
		exit(1);
	}
	struct termios new_termios = org_termios;

	new_termios.c_lflag &= ~(ECHO | ICANON | ISIG);
	new_termios.c_oflag &= ~(OPOST);

	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios) == -1){
		perror("Died during tcsetattr().");
		exit(1);
	}
}

int main(){
	enableRawMode();
	char c;

	while(1){
		read(STDIN_FILENO,&c,1);
		printf("%d (%c)\r\n",c,c);
	}
	return 0;
}