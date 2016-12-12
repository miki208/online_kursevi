#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <stdarg.h>
#include <errno.h>

void error_fatal(char *format, ...);

int main() {
    return 0;
}

void error_fatal (char* format, ...)
{
  va_list arguments;

  va_start (arguments, format);
  vfprintf (stderr, format, arguments);
  va_end (arguments);

  exit (EXIT_FAILURE);
}