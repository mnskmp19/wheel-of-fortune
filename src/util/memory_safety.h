#ifndef _UTIL_MEMORY_SAFETY_H_
#define _UTIL_MEMORY_SAFETY_H_

#include <stddef.h>
#include <stdlib.h>

#define member_size(type, member) (sizeof(((type*)0)->member))

// #define MEMORY_DEBUG_ON

#ifdef MEMORY_DEBUG_ON
  #define MALLOC(size)          (_malloc_debug_ (size,        __FILE__, __LINE__))
  #define REALLOC(chunk, size)  (_realloc_debug_(chunk, size, __FILE__, __LINE__))
  #define FREE(chunk)           (_free_debug_   (chunk,       __FILE__, __LINE__))
#else
  #define MALLOC(size)          (malloc (size       ))
  #define REALLOC(chunk, size)  (realloc(chunk, size))
  #define FREE(chunk)           (free   (chunk      ))
#endif

void* _malloc_debug_ (size_t size,                  const char* FILE, int LINE);
void* _realloc_debug_(void* old_chunk, size_t size, const char* FILE, int LINE);
void  _free_debug_   (void* chunk,                  const char* FILE, int LINE);

void _memory_debug_init_(void);
void _memory_debug_print_(void);
void _memory_debug_finish_(void);

char* STRDUP(const char* str);

#endif