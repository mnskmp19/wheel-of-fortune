#ifndef _GFX_TEX_ARRAY_
#define _GFX_TEX_ARRAY_

#include "gfx/texture.h"
#include "util/generic_impl.h"

struct tex_array {
  bool is_valid;
  GLuint id;
  int width;
  int height;
  int n_channels;
  int unit;
  size_t size; // number of tiles
  enum tex_filtering filtering;
  struct vec_byte bytes;
};

struct tex_array tex_array_create_empty(enum texture_unit unit, enum tex_filtering filtering);
void tex_array_append(struct tex_array* tex_array, const char* input_file_path);
void tex_array_buffer_data(struct tex_array* tex_array);

struct tex_array tex_array_create(
  const char* input_file_paths[],
  size_t n_input,
  enum texture_unit unit,
  enum tex_filtering filtering
);
struct tex_array tex_array_create_default(
  const char* input_file_paths[],
  size_t n_input,
  enum texture_unit unit
);

void tex_array_use(const struct tex_array* tex_array);
void tex_array_destroy(struct tex_array* tex_array);

#endif