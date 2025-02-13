#ifndef _GFX_MESH2D_H_
#define _GFX_MESH2D_H_

#include <cglm/struct.h>
#include "util/vector_tmpl.h"
#include "util/generic_impl.h"
#include "gfx/shader.h"
#include "gfx/texture.h"
#include "gfx/tex_array.h"

struct vertex2d {
  vec2s pos;
  vec2s tex;
  vec4s fore_color;
  vec4s back_color;
  float tex_idx;
};

VECTOR_DECLARE(vec_vertex2d, struct vertex2d);

struct mesh2d {
  enum mesh2d_kind {
    MESH2D_PLAIN,
    MESH2D_INDEXED,
    MESH2D_TEXTURE_IMAGE,
    MESH2D_TEXARRAY_IMAGE,

    MESH2D_KIND_COUNT,
  } kind;

  unsigned vao;
  struct vec_vertex2d verts;
  unsigned vbo;
  struct vec_uint32 indices;
  unsigned ebo;
  
  const struct texture* texture;
  const struct tex_array* tex_array;

  mat4s model_transform;
};

struct mesh2d
mesh2d_create(
  enum mesh2d_kind kind,
  const struct texture* texture,
  const struct tex_array* tex_array
);

void
mesh2d_destroy(struct mesh2d* mesh);

void
mesh2d_clear_vertex_data(struct mesh2d* mesh);

void
mesh2d_append_triangle_plain(
  struct mesh2d* mesh,
  vec2s a, vec2s b, vec2s c,
  vec4s color
);

void
mesh2d_append_rectangle_plain(
  struct mesh2d* mesh,
  vec2s pos_min, vec2s pos_max,
  vec4s color
);

void
mesh2d_append_rectangle_indexed(
  struct mesh2d* mesh,
  vec2s pos_min, vec2s pos_max,
  vec2s tex_min, vec2s tex_max,
  vec4s fore_color, vec4s back_color
);

void
mesh2d_append_rectangle_texture(
  struct mesh2d* mesh,
  vec2s pos_min, vec2s pos_max,
  vec2s tex_min, vec2s tex_max
);

void
mesh2d_append_rectangle_tex_array(
  struct mesh2d* mesh,
  vec2s pos_min, vec2s pos_max,
  vec2s tex_min, vec2s tex_max,
  float tex_idx
);

void
mesh2d_buffer_data(const struct mesh2d *mesh);

void
mesh2d_render(
  const struct mesh2d* mesh,
  const struct shader* shader,
  vec2s resolution
);

#endif