#ifndef _UTIL_UTILS_H_
#define _UTIL_UTILS_H_

#include <stddef.h>

// memory related stuff
#define array_size(a) (sizeof((a)) / sizeof(*(a)))

#define pass_array_safe(a) (a), array_size(a)

#define container_of(ptr, type, member) ({                      \
        const typeof( ((typeof(type) *)0)->member ) *__mptr = (ptr);    \
        (typeof(type) *)((char *)__mptr - offsetof(type,member));})

// random
#define rand_range(min, max) \
      ({ \
        long __min = (min); \
        long __max = (max); \
        __min + rand() % (__max - __min); \
      })

#define frand() ((float)(RAND_MAX - rand()) / (float)RAND_MAX)
#define frand_range(min, max) \
      ({  \
          float __min = (min); \
          float __max = (max); \
          (frand() * (__max - __min) + __min); \
      }) \

// arithmetic
#define max(a,b) \
  ({ typeof (a) _a = (a); \
    typeof (b) _b = (b); \
    _a > _b ? _a : _b; \
  })

#define min(a,b) \
  ({ typeof (a) _a = (a); \
    typeof (b) _b = (b); \
    _a < _b ? _a : _b; \
  })

#define abs(v) \
  ({ typeof(v) _v = (v); \
    _v > 0 ? _v : -_v; \
  })

// clamping etc.
#define clamp(clamp_val, min, max)\
      ({  \
          typeof(clamp_val) __clamp_val = (clamp_val); \
          typeof(clamp_val) __min = (min); \
          typeof(clamp_val) __max = (max); \
          __clamp_val < __min ? __min : __clamp_val > __max ? __max : __clamp_val; \
      })

#define bring_to_zero(btz_val, delta) \
      ({ \
        typeof(btz_val) __btz_val = (btz_val); \
        typeof(btz_val) __delta = (delta); \
        assert(__delta >= 0); \
        __btz_val > 0.0f ? clamp(__btz_val - __delta, 0, __btz_val) : clamp(__btz_val + __delta, __btz_val, 0); \
      })

#define get_lerp_t(value, beginning_value, end_value)  \
  ({ typeof(value) __value = (value); \
    typeof(beginning_value) __beginning_value = (beginning_value); \
    typeof(end_value) __end_value = (end_value); \
    assert(__beginning_value < __end_value); \
    __value < __beginning_value \
      ? 0.0f \
      : __value > __end_value \
        ? 1.0f\
        : (__value - __beginning_value) / (__end_value - __beginning_value); \
  })

// print macros
#define PRINT_PATH "\"%s\""

// rand test
/*
  float min_rand = +INFINITY;
  float max_rand = -INFINITY;
  float total_rand = 0.0f;
  for(size_t i = 0; i < 100000; i++){
    float v = frand_range(0.0f, 1.0f);
    if(v < min_rand) min_rand = v;
    if(v > max_rand) max_rand = v;
    total_rand += v;
  }

  printf("min=%f,max=%f,avg=%f\n", min_rand, max_rand, total_rand / 100000); 
*/

#endif