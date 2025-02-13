#ifndef _GRAPHICS_SHADER_H_
#define _GRAPHICS_SHADER_H_

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <cglm/struct.h>

enum shader_part {
  SHADER_PART_VERTEX,
  SHADER_PART_GEOMETRY,
  SHADER_PART_TESS_CONTROL,
  SHADER_PART_TESS_EVALUATION,
  SHADER_PART_FRAGMENT,

  SHADER_PART_COUNT,
};

struct shader {
  int is_valid;
  GLuint id;
};

struct shader shader_create_vf_from_file(const char* vert_src_path, const char* frag_src_path);

struct shader shader_create_empty(void);
void shader_attach_from_file(struct shader*, const char* src_path, enum shader_part shader_part);
void shader_link(struct shader*);
void shader_destroy(struct shader*);

void shader_set_uniform_int  (const struct shader*, const char* name, int data);
void shader_set_uniform_float(const struct shader*, const char* name, float data);
void shader_set_uniform_vec3f(const struct shader*, const char* name, vec3s data);
void shader_set_uniform_vec4f(const struct shader*, const char* name, vec4s data);
void shader_set_uniform_mat4f(const struct shader*, const char* name, mat4s data);
void shader_set_uniform_mat4f_array(const struct shader*, const char* name, const mat4s* data, size_t size);

#endif