#ifndef _GFX_GFX_UTILS_
#define _GFX_GFX_UTILS_

#include <cglm/struct.h>

// print macros
#define PRINT_SCALAR "%6f"

#define PRINT_VEC2 "(" PRINT_SCALAR "," PRINT_SCALAR ")"
#define PASS_VEC2_PARTS(v) v.x, v.y

#define PRINT_VEC3 "(" PRINT_SCALAR "," PRINT_SCALAR "," PRINT_SCALAR ")"
#define PASS_VEC3_PARTS(v) v.x, v.y, v.z

#define PRINT_VEC4 "(" PRINT_SCALAR "," PRINT_SCALAR "," PRINT_SCALAR "," PRINT_SCALAR ")"
#define PASS_VEC4_PARTS(v) v.x, v.y, v.z, v.w

#define PRINT_MAT4 \
  "(" PRINT_SCALAR "," PRINT_SCALAR "," PRINT_SCALAR "," PRINT_SCALAR ")\n" \
  "(" PRINT_SCALAR "," PRINT_SCALAR "," PRINT_SCALAR "," PRINT_SCALAR ")\n" \
  "(" PRINT_SCALAR "," PRINT_SCALAR "," PRINT_SCALAR "," PRINT_SCALAR ")\n" \
  "(" PRINT_SCALAR "," PRINT_SCALAR "," PRINT_SCALAR "," PRINT_SCALAR ")\n"

#define PASS_MAT4_PARTS(m) \
      m.col[0].raw[0], m.col[1].raw[0], m.col[2].raw[0], m.col[3].raw[0], \
      m.col[0].raw[1], m.col[1].raw[1], m.col[2].raw[1], m.col[3].raw[1], \
      m.col[0].raw[2], m.col[1].raw[2], m.col[2].raw[2], m.col[3].raw[2], \
      m.col[0].raw[3], m.col[1].raw[3], m.col[2].raw[3], m.col[3].raw[3]

static inline vec3s
get_normal(vec3s dir){
  assert(glms_vec3_norm(dir) > 0.0f);   

  vec3s normal = (vec3s){ .x = 0.0f, .y = 0.0f, .z = 0.0f, };
  while(glms_vec3_dot(dir, normal) == 0.0f){
    normal = (vec3s){ .x = frand(), .y = frand(), .z = frand(), };
  }

  return glms_vec3_normalize(glms_vec3_cross(dir, normal));
}

#endif