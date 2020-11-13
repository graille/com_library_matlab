#ifndef USEFUL_LIB_PRINTF_H
#define USEFUL_LIB_PRINTF_H
 
#include <stdlib.h>
#include <math.h>

// Source: https://stackoverflow.com/questions/1961209/making-some-text-in-printf-appear-in-green-and-red
#define _CONSOLE_COLOR_RESET   "\033[0m"
#define _CONSOLE_COLOR_BLACK   "\033[30m"      /* Black */
#define _CONSOLE_COLOR_RED     "\033[31m"      /* Red */
#define _CONSOLE_COLOR_GREEN   "\033[32m"      /* Green */
#define _CONSOLE_COLOR_YELLOW  "\033[33m"      /* Yellow */
#define _CONSOLE_COLOR_BLUE    "\033[34m"      /* Blue */
#define _CONSOLE_COLOR_MAGENTA "\033[35m"      /* Magenta */
#define _CONSOLE_COLOR_CYAN    "\033[36m"      /* Cyan */
#define _CONSOLE_COLOR_WHITE   "\033[37m"      /* White */
#define _CONSOLE_BACKGROUND_COLOR_BLACK   "\033[40m"      /* Black */
#define _CONSOLE_BACKGROUND_COLOR_RED     "\033[41m"      /* Red */
#define _CONSOLE_BACKGROUND_COLOR_GREEN   "\033[42m"      /* Green */
#define _CONSOLE_BACKGROUND_COLOR_YELLOW  "\033[43m"      /* Yellow */
#define _CONSOLE_BACKGROUND_COLOR_BLUE    "\033[44m"      /* Blue */
#define _CONSOLE_BACKGROUND_COLOR_MAGENTA "\033[45m"      /* Magenta */
#define _CONSOLE_BACKGROUND_COLOR_CYAN    "\033[46m"      /* Cyan */
#define _CONSOLE_BACKGROUND_COLOR_WHITE   "\033[47m"      /* White */

#define _CONSOLE_BOLD "\033[1m"
#define _CONSOLE_UNDERLINE "\033[4m"
#define _CONSOLE_FLASH "\033[5m"
#define _CONSOLE_OVERLINE "\033[7m"

#define _CONSOLE_COLOR_BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define _CONSOLE_COLOR_BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define _CONSOLE_COLOR_BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define _CONSOLE_COLOR_BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define _CONSOLE_COLOR_BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define _CONSOLE_COLOR_BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define _CONSOLE_COLOR_BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define _CONSOLE_COLOR_BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#define _printf_error(text, ...) fprintf(stderr, _CONSOLE_COLOR_RED "ERROR: " text _CONSOLE_COLOR_RESET, ##__VA_ARGS__)
#define _printf_info(text, ...) printf(_CONSOLE_COLOR_CYAN "INFO: " text _CONSOLE_COLOR_RESET _CONSOLE_COLOR_RESET, ##__VA_ARGS__)
#define _printf_success(text, ...) printf(_CONSOLE_COLOR_GREEN text _CONSOLE_COLOR_RESET, ##__VA_ARGS__)

#endif // USEFUL_LIB_PRINTF_H

