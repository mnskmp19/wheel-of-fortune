#include "error_handling.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define DEFAULT_ERRORS_COUNT (ERROR_OPENAL + 1)

void critical_error(int value){
  assert(value < DEFAULT_ERRORS_COUNT);

  static const char* default_messages[] = {
    [ERROR_UNKNOWN] = "Unknown error",
    [ERROR_MEMORY]  = "Memory error",
    [ERROR_OPENGL]  = "OpenGL error",
    [ERROR_OPENAL]  = "OpenAL error",
  };

  printf("%s\n", default_messages[value]);
  exit(value);
}

void critical_error_msg(const char* message, int value){
  printf("%s\n", message);
  exit(value);
}