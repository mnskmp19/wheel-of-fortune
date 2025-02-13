#ifndef _UTIL_ERROR_HANDLING_
#define _UTIL_ERROR_HANDLING_

#define ERROR_UNKNOWN 1
#define ERROR_MEMORY  2
#define ERROR_OPENGL  3
#define ERROR_OPENAL  4

extern void critical_error(int value);
extern void critical_error_msg(const char* message, int value);

#endif