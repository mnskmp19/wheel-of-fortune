#ifndef _UTIL_VECTOR_TMPL_H_
#define _UTIL_VECTOR_TMPL_H_

#include <stdint.h>
#include <assert.h>
#include "util/memory_safety.h"
#include "util/error_handling.h"

#define VECTOR_DEFAULT_CAPACITY 4

// don't allow allocating 0 elements
// destroyed vector has data set to NULL (so multiple destructors can be called without error)

#define VECTOR_DECLARE(VECTOR, TYPE) \
struct VECTOR { \
  typeof(TYPE)* data; \
  size_t size; \
  size_t cap; \
}; \
\
struct VECTOR VECTOR ## _create_default(void); \
struct VECTOR VECTOR ## _create(size_t cap); \
void          VECTOR ## _destroy(struct VECTOR* vector); \
void          VECTOR ## _reserve(struct VECTOR* vector, size_t new_cap); \
void          VECTOR ## _resize(struct VECTOR* vector, size_t new_size); \
void          VECTOR ## _clear(struct VECTOR* vector); \
void          VECTOR ## _shrink(struct VECTOR* vector); \
typeof(TYPE)* VECTOR ## _get_last(const struct VECTOR* vector); \
typeof(TYPE)* VECTOR ## _begin(const struct VECTOR* vector); \
typeof(TYPE)* VECTOR ## _end(const struct VECTOR* vector); \
void          VECTOR ## _push(struct VECTOR* vector, typeof(TYPE) value); \
void          VECTOR ## _push_array(struct VECTOR* vector, typeof(TYPE)* values, size_t n_elems); \
void          VECTOR ## _insert(struct VECTOR* vector, size_t index, typeof(TYPE) value); \
void          VECTOR ## _insert_array(struct VECTOR* vector, size_t index, typeof(TYPE)* values, size_t n_elems); \
void          VECTOR ## _pop(struct VECTOR* vector); \
void          VECTOR ## _remove(struct VECTOR* vector, size_t index); \
\
struct VECTOR

#define VECTOR_IMPLEMENT(VECTOR, TYPE) \
struct VECTOR \
VECTOR ## _create(size_t cap){ \
  struct VECTOR vector = (struct VECTOR){ \
    .size = 0, \
  }; \
  VECTOR ## _reserve(&vector, cap); \
  return vector; \
} \
\
struct VECTOR \
VECTOR ## _create_default(void){ \
  return VECTOR ## _create(VECTOR_DEFAULT_CAPACITY); \
} \
\
void \
VECTOR ## _destroy(struct VECTOR* vector){ \
  if(vector->data != NULL) FREE(vector->data); \
  vector->data = NULL; \
} \
\
void \
VECTOR ## _reserve(struct VECTOR* vector, size_t new_cap){ \
  assert(new_cap > 0); \
  assert(vector->size <= new_cap); \
  vector->data = REALLOC(vector->data, sizeof(*vector->data) * new_cap); \
  if(vector->data == NULL) critical_error(ERROR_MEMORY); \
  vector->cap = new_cap; \
} \
\
void \
VECTOR ## _resize(struct VECTOR* vector, size_t new_size){ \
  if(vector->cap < new_size) VECTOR ## _reserve(vector, new_size); \
  vector->size = new_size; \
} \
void VECTOR ## _clear(struct VECTOR* vector){ \
  vector->size = 0; \
} \
\
void \
VECTOR ## _shrink(struct VECTOR* vector){ \
  VECTOR ## _reserve(vector, vector->size > 0 ? vector->size : 1); \
} \
\
typeof(TYPE)* \
VECTOR ## _get_last(const struct VECTOR* vector){ \
  return &vector->data[vector->size - 1]; \
} \
\
typeof(TYPE)* \
VECTOR ## _begin(const struct VECTOR* vector){ \
  return &vector->data[0]; \
} \
\
typeof(TYPE)* \
VECTOR ## _end(const struct VECTOR* vector){ \
  return &vector->data[vector->size]; \
} \
\
static void \
VECTOR ## _should_grow(struct VECTOR* vector, size_t new_size){ \
  if(vector->cap >= new_size) return; \
  size_t calculated_cap = vector->cap * 2 > new_size ? vector->cap * 2 : new_size; \
  VECTOR ## _reserve(vector, calculated_cap); \
} \
\
void \
VECTOR ## _push(struct VECTOR* vector, typeof(TYPE) value){ \
  VECTOR ## _should_grow(vector, vector->size + 1); \
  vector->data[vector->size] = value; \
  vector->size++; \
} \
\
void \
VECTOR ## _push_array(struct VECTOR* vector, typeof(TYPE)* values, size_t n_elems){ \
  VECTOR ## _should_grow(vector, vector->size + n_elems); \
  for(size_t i = 0; i < n_elems; i++){ \
    vector->data[vector->size + i] = values[i]; \
  } \
  vector->size += n_elems; \
} \
\
void \
VECTOR ## _insert(struct VECTOR* vector, size_t index, typeof(TYPE) value){ \
  size_t new_size = (vector->size > index ? vector->size : index) + 1; \
  VECTOR ## _should_grow(vector, new_size); \
  for(size_t i = vector->size; i-- > index; ){ \
    vector->data[i + 1] = vector->data[i];  \
  } \
  vector->data[index] = value; \
  vector->size = new_size; \
} \
\
void \
VECTOR ## _insert_array(struct VECTOR* vector, size_t index, typeof(TYPE)* values, size_t n_elems){ \
  size_t new_size = (vector->size > index ? vector->size : index) + n_elems; \
  VECTOR ## _should_grow(vector, new_size); \
  for(size_t i = vector->size; i-- > index; ){ \
    vector->data[i + n_elems] = vector->data[i];  \
  } \
  for(size_t i = 0; i < n_elems; i++){ \
    vector->data[index + i] = values[i]; \
  } \
  vector->size = new_size; \
} \
\
void \
VECTOR ## _remove(struct VECTOR* vector, size_t index){ \
  assert(index < vector->size); \
  for(size_t i = index; i < vector->size - 1; i++){ \
    vector->data[i] = vector->data[i + 1]; \
  } \
  vector->size--; \
} \
void VECTOR ## _pop(struct VECTOR* vector) {\
  assert(vector->size > 0); \
  vector->size--; \
}\
\
struct VECTOR

#endif