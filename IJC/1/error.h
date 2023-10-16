/* 
 error.h
 Autor: Lukáš Neupauer xneupa01
 Project: IJC DU1
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


void warning_msg(const char *fmt, ...);
void error_exit(const char *fmt, ...);