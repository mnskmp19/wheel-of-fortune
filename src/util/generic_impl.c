#include "vector_tmpl.h"

#include "util/generic_impl.h"

#include "gfx/mesh2d.h"

// util/generic_impl.h
VECTOR_IMPLEMENT(vec_byte, unsigned char);
VECTOR_IMPLEMENT(vec_size_t, size_t);
VECTOR_IMPLEMENT(vec_uint32, uint32_t);
VECTOR_IMPLEMENT(strbuff, char);
VECTOR_IMPLEMENT(vec_strbuff, struct strbuff);

VECTOR_IMPLEMENT(vec_vec4s, vec4s);
VECTOR_IMPLEMENT(vec_mat4s, mat4s);

// gfx/mesh2d.h
VECTOR_IMPLEMENT(vec_vertex2d, struct vertex2d);