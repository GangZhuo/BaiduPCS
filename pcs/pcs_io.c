#include <stdio.h>
#include <stdlib.h>

#include "pcs_io.h"


#ifdef WIN32

void get_string_from_std_input(char *str, int size)
{
	char c;
	int i = 0;

	while ((c = _getch()) != '\r' && c != '\n') {
		if (c == '\b') {
			if (i > 0) {
				i--;
				putchar(c);
				putchar(' ');
				putchar(c);
			}
		}
		else if (isprint(c)) {
			str[i] = c;
			putchar(c);
			i++;
			if (i >= size) {
				break;
			}
		}
	}
	str[i >= size ? (size - 1) : i] = '\0';
	printf("\n");
}

void get_password_from_std_input(char *password, int size)
{
	char c;
	int i = 0;

	while ((c = _getch()) != '\r' && c != '\n') {
		if (c == '\b') {
			if (i > 0)
				i--;
		}
		else if (isprint(c)) {
			password[i] = c;
			//putchar('*');
			i++;
			if (i >= size) {
				break;
			}
		}
	}
	password[i >= size ? (size - 1) : i] = '\0';
	printf("\n");
}

#else
#include <unistd.h>

void get_username_from_std_input(char *str, int size)
{
	struct termios oflags, nflags;

	/* disabling echo */
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    //nflags.c_lflag &= ~ECHO;
    //nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
        perror("tcsetattr");
        return;
    }

    //printf("user name: ");
    fgets(str, size, stdin);
    str[size - 1] = 0;
    str[strlen(str) - 1] = 0;
    //printf("you typed '%s'\n", str);

    /* restore terminal */
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
        perror("tcsetattr");
        return;
    }
}

void get_password_from_std_input(char *password, int size)
{
	struct termios oflags, nflags;

	/* disabling echo */
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
        perror("tcsetattr");
        return;
    }

    //printf("password: ");
    fgets(password, size, stdin);
    password[size - 1] = 0;
    password[strlen(password) - 1] = 0;
    //printf("you typed '%s'\n", password);

    /* restore terminal */
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
        perror("tcsetattr");
        return;
    }

}

#endif

