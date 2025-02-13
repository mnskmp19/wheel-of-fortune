#include "memory_safety.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "util/error_handling.h"

struct memory_debug_frame {
  void* chunk;
  const char* FILE;
  int LINE;
  int freed;
};

static size_t memory_frames_size;
static size_t memory_frames_allocated;
static struct memory_debug_frame* memory_frames;

static void insert_memory_frame(struct memory_debug_frame new_frame){
  if(memory_frames_size == memory_frames_allocated){
    memory_frames_allocated *= 2;
    memory_frames = realloc(memory_frames, memory_frames_allocated * sizeof(struct memory_debug_frame));
  }

  memory_frames[memory_frames_size++] = new_frame;
}

void* _malloc_debug_(size_t size, const char* FILE, int LINE){
  void* new_chunk = malloc(size);

  // tag new chunk as allocated
  for(size_t i = 0; i < memory_frames_size; i++){
    if(memory_frames[i].chunk == new_chunk) {
      memory_frames[i] = (struct memory_debug_frame){
        .chunk = new_chunk,
        .FILE  = FILE,
        .LINE  = LINE,
        .freed = 0,
      };
      return new_chunk;
    }
  }

  insert_memory_frame((struct memory_debug_frame){
    .chunk = new_chunk,
    .FILE  = FILE,
    .LINE  = LINE,
    .freed = 0,
  });

  return new_chunk;
}

void* _realloc_debug_(void* old_chunk, size_t size, const char* FILE, int LINE){
  void* new_chunk = realloc(old_chunk, size);
  if(new_chunk == old_chunk){
    return new_chunk;
  }

  // tag old chunk as free
  for(size_t i = 0; i < memory_frames_size; i++){
    if(memory_frames[i].chunk == old_chunk) {
      memory_frames[i].freed = 1;
      break;
    }
  }

  // tag new chunk as allocated
  for(size_t i = 0; i < memory_frames_size; i++){
    if(memory_frames[i].chunk == new_chunk) {
      memory_frames[i] = (struct memory_debug_frame){
        .chunk = new_chunk,
        .FILE  = FILE,
        .LINE  = LINE,
        .freed = 0,
      };
      return new_chunk;
    }
  }

  insert_memory_frame((struct memory_debug_frame){
    .chunk = new_chunk,
    .FILE  = FILE,
    .LINE  = LINE,
    .freed = 0,
  });

  return new_chunk;
}

void _free_debug_(void* chunk, const char* FILE, int LINE){
  (void)FILE; // supress warnings
  (void)LINE; // supress warnings

  free(chunk);

  // tag old chunk as free
  for(size_t i = 0; i < memory_frames_size; i++){
    if(memory_frames[i].chunk == chunk) {
      memory_frames[i].freed = 1;
      return;
    }
  }
}

void _memory_debug_init_(void){
  #ifdef MEMORY_DEBUG_ON
  memory_frames_size = 0;
  memory_frames_allocated = 4;
  memory_frames = realloc(NULL, memory_frames_allocated * sizeof(struct memory_debug_frame));
  #endif
}

void _memory_debug_print_(void){
  #ifdef MEMORY_DEBUG_ON
  printf("TOTAL CHUNKS ALLOCATED: %"PRIu64"\n", memory_frames_size);

  int memory_leaks = 0;

  for(size_t i = 0; i < memory_frames_size; i++){
    struct memory_debug_frame* current_frame = &memory_frames[i];
    if(current_frame->freed == 0){
      memory_leaks++;
      printf("-MEMORY LEAK at address %p allocated in file %s:%d\n", 
        current_frame->chunk,
        current_frame->FILE,
        current_frame->LINE
      );
    }
  }

  printf("TOTAL MEMORY LEAKS DETECTED: %d\n", memory_leaks);
  #endif
}

void _memory_debug_finish_(void){
  #ifdef MEMORY_DEBUG_ON
  memory_frames_size = 0;
  memory_frames_allocated = 0;
  free(memory_frames);
  #endif
}

char* STRDUP(const char* str){
  if(str == NULL) return NULL;
  char* ret = MALLOC(strlen(str) + 1);
  if(ret == NULL) critical_error(ERROR_MEMORY);
  strcpy(ret, str);

  return ret;
}