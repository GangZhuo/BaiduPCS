#ifndef _PCS_IO_H
#define _PCS_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef WIN32
#  include <malloc.h>
#  include <conio.h>
#  include <direct.h>
#  define strcmpi  _strcmpi
#  define strncmpi _strnicmp
#  define snprintf _snprintf
#  define mkdir _mkdir
#else
#  include <alloca.h>
#  include <inttypes.h>
#  include <termios.h>
#endif

void get_string_from_std_input(char *str, int size);
void get_password_from_std_input(char *password, int size);

#endif
