#include "gfx/mesh2d.h"

#include "util/utils.h"

struct mesh2d
mesh2d_create(
  enum mesh2d_kind kind,
  const struct texture* texture,
  const struct tex_array* tex_array
){
  switch(kind){
  case MESH2D_PLAIN:          assert(texture == NULL && tex_array == NULL); break;
  case MESH2D_INDEXED:        assert(texture != NULL && tex_array == NULL); break;
  case MESH2D_TEXTURE_IMAGE:  assert(texture != NULL && tex_array == NULL); break;
  case MESH2D_TEXARRAY_IMAGE: assert(texture == NULL && tex_array != NULL); break;
  default: assert(0);
  }

  struct mesh2d mesh = {
    .kind = kind,

    .vao = 0,
    .verts = vec_vertex2d_create_default(),
    .vbo = 0,
    .indices = vec_uint32_create_default(),
    .ebo = 0,

    .texture = texture,
    .tex_array = tex_array,

    .model_transform = glms_mat4_identity(),
  };

  glCreateVertexArrays(1, &mesh.vao);
  if(mesh.vao == 0) critical_error(ERROR_OPENGL);
  glCreateBuffers(1, &mesh.vbo);
  if(mesh.vbo == 0) critical_error(ERROR_OPENGL);
  glCreateBuffers(1, &mesh.ebo);
  if(mesh.ebo == 0) critical_error(ERROR_OPENGL);

    // bind VAO
  glBindVertexArray(mesh.vao);
  // bind VBO
  glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
  // bind EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
  // enable VAO attribs
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex2d), (void*)offsetof(struct vertex2d, pos));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex2d), (void*)offsetof(struct vertex2d, tex));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(struct vertex2d), (void*)offsetof(struct vertex2d, fore_color));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(struct vertex2d), (void*)offsetof(struct vertex2d, back_color));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(struct vertex2d), (void*)offsetof(struct vertex2d, tex_idx));
  glEnableVertexAttribArray(4);

  // unbind VAO
  glBindVertexArray(0);

  return mesh;
}

void
mesh2d_destroy(struct mesh2d *mesh){
  glDeleteVertexArrays(1, &mesh->vao);
  vec_vertex2d_destroy(&mesh->verts);
  glDeleteBuffers(1, &mesh->vbo);
  vec_uint32_destroy(&mesh->indices);
  glDeleteBuffers(1, &mesh->ebo);
}

void
mesh2d_clear_vertex_data(struct mesh2d *mesh){
  vec_vertex2d_resize(&mesh->verts, 0);
  vec_uint32_resize(&mesh->indices, 0);
}

void
mesh2d_append_triangle_plain(
  struct mesh2d *mesh,
  vec2s a, vec2s b, vec2s c,
  vec4s color
){
  assert(mesh->kind == MESH2D_PLAIN);

  struct vertex2d vertex_a = {
    .pos = a,
    .fore_color = color,
  };
  struct vertex2d vertex_b = {
    .pos = b,
    .fore_color = color,
  };
  struct vertex2d vertex_c = {
    .pos = c,
    .fore_color = color,
  };

  size_t n_verts = mesh->verts.size;
  vec_vertex2d_push_array(
    &mesh->verts,
    pass_array_safe((
      (struct vertex2d[]){ vertex_a, vertex_b, vertex_c, }
    ))
  );

  vec_uint32_push_array(
    &mesh->indices,
    pass_array_safe((
      (uint32_t[]){ n_verts + 0, n_verts + 1, n_verts + 2, }
    ))
  );
}

void
mesh2d_append_rectangle_plain(
  struct mesh2d *mesh,
  vec2s pos_min, vec2s pos_max,
  vec4s color
){
  assert(mesh->kind == MESH2D_PLAIN);
  
  struct vertex2d lower_left = {
    .pos = { .x = pos_min.x, .y = pos_min.y, },
    .fore_color = color,
  };
  struct vertex2d lower_right = {
    .pos = { .x = pos_max.x, .y = pos_min.y, },
    .fore_color = color,
  };
  struct vertex2d upper_left = {
    .pos = { .x = pos_min.x, .y = pos_max.y, },
    .fore_color = color,
  };
  struct vertex2d upper_right = {
    .pos = { .x = pos_max.x, .y = pos_max.y, },
    .fore_color = color,
  };

  size_t n_verts = mesh->verts.size;
  vec_vertex2d_push_array(
    &mesh->verts,
    pass_array_safe((
      (struct vertex2d[]) { lower_left, lower_right, upper_right, upper_left, }
    ))
  );
  vec_uint32_push_array(
    &mesh->indices,
    pass_array_safe((
      (uint32_t[]) { n_verts + 0, n_verts + 1, n_verts + 2, n_verts + 2, n_verts + 3, n_verts + 0, }
    ))
  );
}

void
mesh2d_append_rectangle_indexed(
  struct mesh2d *mesh,
  vec2s pos_min, vec2s pos_max,
  vec2s tex_min, vec2s tex_max,
  vec4s fore_color, vec4s back_color
){

  struct vertex2d lower_left = {
    .pos = { .x = pos_min.x, .y = pos_min.y, },
    .tex = { .u = tex_min.x, .v = tex_max.y, },
    .fore_color = fore_color,
    .back_color = back_color,
  };
  struct vertex2d lower_right = {
    .pos = { .x = pos_max.x, .y = pos_min.y, },
    .tex = { .u = tex_max.x, .v = tex_max.y, },
    .fore_color = fore_color,
    .back_color = back_color,
  };
  struct vertex2d upper_left = {
    .pos = { .x = pos_min.x, .y = pos_max.y, },
    .tex = { .u = tex_min.x, .v = tex_min.y, },
    .fore_color = fore_color,
    .back_color = back_color,
  };
  struct vertex2d upper_right = {
    .pos = { .x = pos_max.x, .y = pos_max.y, },
    .tex = { .u = tex_max.x, .v = tex_min.y, },
    .fore_color = fore_color,
    .back_color = back_color,
  };

  size_t n_verts = mesh->verts.size;
  vec_vertex2d_push_array(
    &mesh->verts,
    pass_array_safe((
      (struct vertex2d[]) { lower_left, lower_right, upper_right, upper_left, }
    ))
  );
  vec_uint32_push_array(
    &mesh->indices,
    pass_array_safe((
      (uint32_t[]) { n_verts + 0, n_verts + 1, n_verts + 2, n_verts + 2, n_verts + 3, n_verts + 0, }
    ))
  );
}

void
mesh2d_append_rectangle_texture(
  struct mesh2d *mesh,
  vec2s pos_min, vec2s pos_max,
  vec2s tex_min, vec2s tex_max
){

  assert(mesh->kind == MESH2D_TEXTURE_IMAGE);

  struct vertex2d lower_left = {
    .pos = { .x = pos_min.x, .y = pos_min.y, },
    .tex = { .u = tex_min.x, .v = tex_max.y, },
  };
  struct vertex2d lower_right = {
    .pos = { .x = pos_max.x, .y = pos_min.y, },
    .tex = { .u = tex_max.x, .v = tex_max.y, },
  };
  struct vertex2d upper_left = {
    .pos = { .x = pos_min.x, .y = pos_max.y, },
    .tex = { .u = tex_min.x, .v = tex_min.y, },
  };
  struct vertex2d upper_right = {
    .pos = { .x = pos_max.x, .y = pos_max.y, },
    .tex = { .u = tex_max.x, .v = tex_min.y, },
  };

  size_t n_verts = mesh->verts.size;
  vec_vertex2d_push_array(
    &mesh->verts,
    pass_array_safe((
      (struct vertex2d[]) { lower_left, lower_right, upper_right, upper_left, }
    ))
  );
  vec_uint32_push_array(
    &mesh->indices,
    pass_array_safe((
      (uint32_t[]) { n_verts + 0, n_verts + 1, n_verts + 2, n_verts + 2, n_verts + 3, n_verts + 0, }
    ))
  );
}

void
mesh2d_append_rectangle_tex_array(
  struct mesh2d *mesh,
  vec2s pos_min, vec2s pos_max,
  vec2s tex_min, vec2s tex_max,
  float tex_idx
){

  assert(mesh->kind == MESH2D_TEXARRAY_IMAGE);

  struct vertex2d lower_left = {
    .pos = { .x = pos_min.x, .y = pos_min.y, },
    .tex = { .u = tex_min.x, .v = tex_max.y, },
    .tex_idx = tex_idx,
  };
  struct vertex2d lower_right = {
    .pos = { .x = pos_max.x, .y = pos_min.y, },
    .tex = { .u = tex_max.x, .v = tex_max.y, },
    .tex_idx = tex_idx,
  };
  struct vertex2d upper_left = {
    .pos = { .x = pos_min.x, .y = pos_max.y, },
    .tex = { .u = tex_min.x, .v = tex_min.y, },
    .tex_idx = tex_idx,
  };
  struct vertex2d upper_right = {
    .pos = { .x = pos_max.x, .y = pos_max.y, },
    .tex = { .u = tex_max.x, .v = tex_min.y, },
    .tex_idx = tex_idx,
  };

  size_t n_verts = mesh->verts.size;
  vec_vertex2d_push_array(
    &mesh->verts,
    pass_array_safe((
      (struct vertex2d[]) { lower_left, lower_right, upper_right, upper_left, }
    ))
  );
  vec_uint32_push_array(
    &mesh->indices,
    pass_array_safe((
      (uint32_t[]) { n_verts + 0, n_verts + 1, n_verts + 2, n_verts + 2, n_verts + 3, n_verts + 0, }
    ))
  );
}

void
mesh2d_buffer_data(const struct mesh2d *mesh){

  // buffer vertices
  glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
  glBufferData(
    GL_ARRAY_BUFFER,
    mesh->verts.size * sizeof(*mesh->verts.data),
    mesh->verts.data,
    GL_STATIC_DRAW
  );
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  // buffer indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    mesh->indices.size * sizeof(*mesh->indices.data),
    mesh->indices.data,
    GL_STATIC_DRAW
  );
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void
mesh2d_render(
  const struct mesh2d *mesh,
  const struct shader *shader,
  vec2s resolution
){

  assert(resolution.x > 0.0f && resolution.y > 0.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);

  // RENDER GEOMETRY
  mat4s model_matrix = glms_mat4_identity();
  model_matrix = glms_translate(model_matrix, (vec3s){ .x = -1.0, .y = -1.0f, });
  model_matrix = glms_scale(model_matrix, (vec3s){ .x = 2.0f / resolution.x, .y = 2.0f / resolution.y, 1.0f, });
  model_matrix = glms_mat4_mul(model_matrix, mesh->model_transform);
  shader_set_uniform_mat4f(shader, "model", model_matrix);

  // bind
  glUseProgram(shader->id);
  glBindVertexArray(mesh->vao);

  switch(mesh->kind){
  case MESH2D_PLAIN: 
    break;
  case MESH2D_INDEXED: 
    texture_use(mesh->texture);
    break;
  case MESH2D_TEXTURE_IMAGE:
    texture_use(mesh->texture);
    break;
  case MESH2D_TEXARRAY_IMAGE:
    tex_array_use(mesh->tex_array);
    break;
  default: assert(0);
  }

  // call draw function
  glDrawElements(GL_TRIANGLES, mesh->indices.size, GL_UNSIGNED_INT, 0);
  
  // unbind
  glUseProgram(0);
  glBindVertexArray(0);

  switch(mesh->kind){
  case MESH2D_PLAIN: 
    break;
  case MESH2D_INDEXED: 
    texture_use(NULL);
    break;
  case MESH2D_TEXTURE_IMAGE:
    texture_use(NULL);
    break;
  case MESH2D_TEXARRAY_IMAGE:
    tex_array_use(NULL);
    break;
  default: assert(0);
  }
  
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
}