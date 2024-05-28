#include "um245.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * serial_writeline_f is like serial_writeline but accepts a format string.
 * Example usage:
 * serial_writeline_f("find_node block num %d\n", block_num); 
 */
void serial_writeline_f(const char *fmt, ...) {
  char *data = (char *)malloc(1024 * sizeof(char));
  va_list arg;
  va_start(arg, fmt);
  vsprintf(data, fmt, arg);
  serial_writeline(data);
  va_end(arg);
  free(data);
}