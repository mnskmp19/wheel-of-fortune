#ifndef _UTIL_GENERIC_IMPL_H_
#define _UTIL_GENERIC_IMPL_H_

#include "util/vector_tmpl.h"

#include <stdint.h>
#include <cglm/struct.h>

// use to declare templates of standard data types
// templates of non-standard data types 
//   are declared in the same file as the data file

VECTOR_DECLARE(vec_byte, unsigned char);
VECTOR_DECLARE(vec_size_t, size_t);
VECTOR_DECLARE(vec_uint32, uint32_t);
VECTOR_DECLARE(strbuff, char); // not zero terminated
VECTOR_DECLARE(vec_strbuff, struct strbuff);

VECTOR_DECLARE(vec_vec4s, vec4s);
VECTOR_DECLARE(vec_mat4s, mat4s);

#endif