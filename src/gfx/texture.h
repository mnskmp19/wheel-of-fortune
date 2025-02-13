#ifndef _GRAPHICS_TEXTURE_H_
#define _GRAPHICS_TEXTURE_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

struct texture {
  bool is_valid;
  GLuint id;
  int width;
  int height;
  int n_channels;
  int unit;
};

enum texture_unit {
  TEXTURE_DIFFUSE
};

enum tex_filtering {
  TEX_FILTERING_NONE,
  TEX_FILTERING_LINEAR,
  TEX_FILTERING_BILINEAR,
  TEX_FILTERING_TRILINEAR,
};

#define TEX_DEFAULT_FILTERING TEX_FILTERING_NONE
// #define TEX_DEFAULT_FILTERING TEX_FILTERING_TRILINEAR

struct texture texture_create(
  const char* input_file_path,
  enum texture_unit unit,
  enum tex_filtering filtering
);
struct texture texture_create_default(
  const char* input_file_path, 
  enum texture_unit unit
);

struct texture texture_create_from_buffer(
  const void* buffer,
  size_t size,
  enum texture_unit unit,
  enum tex_filtering filtering
);
struct texture texture_create_default_from_buffer(
  const void* buffer,
  size_t size,
  enum texture_unit unit
);

void texture_use(const struct texture* texture);
void texture_destroy(struct texture* texture);

#endif