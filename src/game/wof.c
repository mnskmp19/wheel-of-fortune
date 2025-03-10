#include "wof.h"

#include <assert.h>
#include "input/window.h"
#include "util/utils.h"

#define RESOLUTION_X 4.0f
#define RESOLUTION_Y (RESOLUTION_X / ASPECT_RATIO)

#define WOF_FRICTION 2.0f
#define WOF_MIN_SPEED 8.0f
#define WOF_MAX_SPEED 12.0f

//#7effc6
//#295a4f

#define COLOR_VEC4_FROM_HEX(hex) { \
  .r = (float)(((unsigned)(hex) & 0xff0000) >> 16) / 256.0f, \
  .g = (float)(((unsigned)(hex) & 0x00ff00) >>  8) / 256.0f, \
  .b = (float)(((unsigned)(hex) & 0x0000ff) >>  0) / 256.0f, \
  .a = 1.0f, \
 }

static const vec4s wof_colors_back[WOF_PARTS] = {
  COLOR_VEC4_FROM_HEX(0x295a4f),
  COLOR_VEC4_FROM_HEX(0x7effc6),
  COLOR_VEC4_FROM_HEX(0x7effc6),
  COLOR_VEC4_FROM_HEX(0x7effc6),
  COLOR_VEC4_FROM_HEX(0x7effc6),
};

static const vec4s wof_colors_front[WOF_PARTS] = {
  COLOR_VEC4_FROM_HEX(0x7effc6),
  COLOR_VEC4_FROM_HEX(0x295a4f),
  COLOR_VEC4_FROM_HEX(0x295a4f),
  COLOR_VEC4_FROM_HEX(0x295a4f),
  COLOR_VEC4_FROM_HEX(0x295a4f),
};

static const vec4s wof_border_color = COLOR_VEC4_FROM_HEX(0x1c1c1c);

static struct texture wof_textures[WOF_PARTS] = { 0 };

static const char *wof_textures_paths[WOF_PARTS] = {
  "res/wheel-of-fortune-00.png",
  "res/wheel-of-fortune-01.png",
  "res/wheel-of-fortune-02.png",
  "res/wheel-of-fortune-03.png",
  "res/wheel-of-fortune-04.png",
};

static struct texture wof_texture_logo = { 0 };

static const char *wof_texture_logo_path = "res/htec-eesy.png";

static struct shader
shader_mesh2d_plain = { 0 };

static struct shader
shader_mesh2d_image = { 0 };

static struct shader
shader_mesh2d_index = { 0 };

void
wof_assets_init(void){
  for(size_t i = 0; i < WOF_PARTS; i++){
    assert(wof_textures_paths[i] != NULL);
    wof_textures[i] = texture_create(
      wof_textures_paths[i],
      TEXTURE_DIFFUSE,
      TEX_FILTERING_TRILINEAR
    );
  }
  assert(wof_texture_logo_path != NULL);
  wof_texture_logo = texture_create(
    wof_texture_logo_path,
    TEXTURE_DIFFUSE,
    TEX_FILTERING_TRILINEAR
  );

  shader_mesh2d_plain = shader_create_vf_from_file("src/glsl/plain2d_vert.glsl", "src/glsl/plain2d_frag.glsl");
  shader_mesh2d_image = shader_create_vf_from_file("src/glsl/image2d_vert.glsl", "src/glsl/image2d_frag.glsl");
  shader_mesh2d_index = shader_create_vf_from_file("src/glsl/index2d_vert.glsl", "src/glsl/index2d_frag.glsl");
}

bool
wof_assets_check_if_valid(void){
  for(size_t i = 0; i < WOF_PARTS; i++){
    if(!wof_textures[i].is_valid) return false;
  }
  if(!wof_texture_logo.is_valid) return false;

  if(!shader_mesh2d_plain.is_valid) return false;
  if(!shader_mesh2d_image.is_valid) return false;
  if(!shader_mesh2d_index.is_valid) return false;

  return true;
}

void
wof_assets_free(void){
  for(size_t i = 0; i < WOF_PARTS; i++){
    texture_destroy(&wof_textures[i]);
  }
  texture_destroy(&wof_texture_logo);
}


#define TRIS_PER_CIRCLE 120

static void
wof_append_image(struct wof *wof,  size_t tex_idx, float r, float angle, vec2s image_size){
  vec2s lower_left  = { .x = -image_size.x / 2.0f, .y = -image_size.y / 2.0f };
  vec2s lower_right = { .x = +image_size.x / 2.0f, .y = -image_size.y / 2.0f };
  vec2s upper_right = { .x = +image_size.x / 2.0f, .y = +image_size.y / 2.0f };
  vec2s upper_left  = { .x = -image_size.x / 2.0f, .y = +image_size.y / 2.0f };

  lower_left  = glms_vec2_rotate(glms_vec2_add(lower_left,  (vec2s){ .x = 0.0f, .y = r, }), angle);
  lower_right = glms_vec2_rotate(glms_vec2_add(lower_right, (vec2s){ .x = 0.0f, .y = r, }), angle);
  upper_right = glms_vec2_rotate(glms_vec2_add(upper_right, (vec2s){ .x = 0.0f, .y = r, }), angle);
  upper_left  = glms_vec2_rotate(glms_vec2_add(upper_left,  (vec2s){ .x = 0.0f, .y = r, }), angle);

  assert(tex_idx < WOF_PARTS);

  struct mesh2d *mesh = &wof->wheel_images[tex_idx];
  assert(mesh->kind == MESH2D_INDEXED);

  size_t n = mesh->verts.size;
  vec_vertex2d_push(&mesh->verts, (struct vertex2d){
    .pos = lower_left, .tex = (vec2s){ .u = 0.0f, .v = 1.0f, },
    .fore_color = wof_colors_front[tex_idx],
    .back_color = (vec4s){ .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 0.0f, },
  });
  vec_vertex2d_push(&mesh->verts, (struct vertex2d){
    .pos = lower_right, .tex = (vec2s){ .u = 1.0f, .v = 1.0f, },
    .fore_color = wof_colors_front[tex_idx],
    .back_color = (vec4s){ .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 0.0f, },
  });
  vec_vertex2d_push(&mesh->verts, (struct vertex2d){
    .pos = upper_right, .tex = (vec2s){ .u = 1.0f, .v = 0.0f, },
    .fore_color = wof_colors_front[tex_idx],
    .back_color = (vec4s){ .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 0.0f, },
  });
  vec_vertex2d_push(&mesh->verts, (struct vertex2d){
    .pos = upper_left, .tex = (vec2s){ .u = 0.0f, .v = 0.0f, },
    .fore_color = wof_colors_front[tex_idx],
    .back_color = (vec4s){ .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 0.0f, },
  });

  vec_uint32_push_array(&mesh->indices, pass_array_safe(
    ((uint32_t[]){
      n + 0, n + 1, n + 2, n + 2, n + 3, n + 0, 
    }))
  );
}

static void
wof_append_sector(struct wof *wof, float r, float start_angle, float end_angle, vec4s color){

  assert(r > 0.0f);
  assert(end_angle > start_angle);
  size_t parts = (end_angle - start_angle) / (2.0f * M_PI) * TRIS_PER_CIRCLE;

  for(size_t i = 0; i < parts; i++){
    mesh2d_append_triangle_plain(
      &wof->wheel_geometry,
      (vec2s){ .x = 0.0f, .y = 0.0f, },
      glms_vec2_rotate((vec2s){ .x = 0.0f, .y = r, }, start_angle +  i    * (2.0f * M_PI) / TRIS_PER_CIRCLE),
      glms_vec2_rotate((vec2s){ .x = 0.0f, .y = r, }, start_angle + (i+1) * (2.0f * M_PI) / TRIS_PER_CIRCLE),
      color
    );
  }

}

static void
wof_append_sector_border(struct wof *wof, float r, float start_angle, float end_angle, vec4s color){

  assert(r > 0.0f);
  assert(end_angle > start_angle);
  size_t parts = (end_angle - start_angle) / (2.0f * M_PI) * TRIS_PER_CIRCLE;

  for(size_t i = 0; i < parts; i++){
    
    mesh2d_append_triangle_plain(
      &wof->wheel_geometry,
      (vec2s){ .x = 0.0f, .y = 0.0f, },
      glms_vec2_rotate((vec2s){ .x = 0.0f, .y = r, }, start_angle +  i    * (2.0f * M_PI) / TRIS_PER_CIRCLE),
      glms_vec2_rotate((vec2s){ .x = 0.0f, .y = r, }, start_angle + (i+1) * (2.0f * M_PI) / TRIS_PER_CIRCLE),
      color
    );
  }
  // fill in the last triangle so the border is symmetric
  mesh2d_append_triangle_plain(
    &wof->wheel_geometry,
    (vec2s){ .x = 0.0f, .y = 0.0f, },
    glms_vec2_rotate((vec2s){ .x = 0.0f, .y = r, }, start_angle + parts * (2.0f * M_PI) / TRIS_PER_CIRCLE),
    glms_vec2_rotate((vec2s){ .x = 0.0f, .y = r, }, end_angle),
    color
  );

  vec2s p = glms_vec2_rotate((vec2s){ .x = 0.0f, .y = r, }, start_angle + (end_angle - start_angle) / 2.0f);
  vec2s A = glms_vec2_rotate((vec2s){ .x = 0.0f, .y = r, }, start_angle);
  vec2s B = glms_vec2_rotate((vec2s){ .x = 0.0f, .y = r, }, end_angle);
  vec2s C = glms_vec2_scale(
    p,
    glms_vec2_dot(A, p) 
      / (glms_vec2_norm(A) * glms_vec2_norm(p))
  );
  vec2s Ap = glms_vec2_sub(A, C);
  vec2s Bp = glms_vec2_sub(B, C);

  mesh2d_append_triangle_plain(
    &wof->wheel_geometry,
    (vec2s){ .x = 0.0f, .y = 0.0f, }, Ap, A,
    color
  );
  mesh2d_append_triangle_plain(
    &wof->wheel_geometry,
    (vec2s){ .x = 0.0f, .y = 0.0f, }, B, Bp,
    color
  );

}

static void
wof_append_static_circle(struct wof *wof, float r, vec4s color){
  assert(r > 0.0f);
  
  float start_angle = 0.0f;
  float end_angle = glm_rad(360.0f);
  size_t parts = (end_angle - start_angle) / (2.0f * M_PI) * TRIS_PER_CIRCLE;

  for(size_t i = 0; i < parts; i++){
    mesh2d_append_triangle_plain(
      &wof->wheel_geometry,
      (vec2s){ .x = 0.0f, .y = 0.0f, },
      glms_vec2_rotate((vec2s){ .x = 0.0f, .y = r, }, start_angle +  i    * (2.0f * M_PI) / TRIS_PER_CIRCLE),
      glms_vec2_rotate((vec2s){ .x = 0.0f, .y = r, }, start_angle + (i+1) * (2.0f * M_PI) / TRIS_PER_CIRCLE),
      color
    );
  }
}

static void
wof_append_static_image(struct wof *wof, vec2s size, vec4s fore_color, vec4s back_color){

  mesh2d_append_rectangle_indexed(
    &wof->static_image,
    glms_vec2_scale(size, -0.5f),
    glms_vec2_scale(size, +0.5f),
    (vec2s){ .u = 0.0f, .v = 0.0f, },
    (vec2s){ .u = 1.0f, .v = 1.0f, },
    fore_color,
    back_color
  );
}

#define SECTOR_BORDER_WIDTH 3.0f

struct wof
wof_create(void){
  struct wof wof = {
    .wheel_geometry = mesh2d_create(MESH2D_PLAIN, NULL, NULL),
    .static_geometry = mesh2d_create(MESH2D_PLAIN, NULL, NULL),
    .static_image = mesh2d_create(MESH2D_TEXTURE_IMAGE, &wof_texture_logo, NULL),

    .state = WOF_STATE_STATIC,
    .angle = 0.0f,
    .speed = 0.0f,
  };

  for(size_t i = 0; i < WOF_PARTS; i++){
    // wof.images[i] = mesh2d_create(MESH2D_TEXTURE_IMAGE, &wof_textures[i], NULL);
    wof.wheel_images[i] = mesh2d_create(MESH2D_INDEXED, &wof_textures[i], NULL);
  }

  mesh2d_append_triangle_plain(
    &wof.static_geometry,
    (vec2s){ .x = +0.0f, .y = 0.9f, },
    (vec2s){ .x = +0.1f, .y = 1.1f, },
    (vec2s){ .x = -0.1f, .y = 1.1f, },
    (vec4s){ .r = 0.6f, .g = 0.6f, .b = 0.6f, .a = 1.0f, }
  );

  for(size_t i = 0; i < WOF_PARTS; i++){
    wof_append_sector(
      &wof,
      1.0f,
      glm_rad( i    * 360.0f / WOF_PARTS),
      glm_rad((i+1) * 360.0f / WOF_PARTS),
      wof_colors_back[i]
    );
  }

  for(size_t i = 0; i < WOF_PARTS; i++){
    wof_append_sector_border(
      &wof,
      1.0f,
      glm_rad(i * 360.0f / WOF_PARTS - SECTOR_BORDER_WIDTH / 2.0f),
      glm_rad(i * 360.0f / WOF_PARTS + SECTOR_BORDER_WIDTH / 2.0f),
      wof_border_color
    );
  }
    
  // append images manually
  wof_append_image(&wof, 0, 0.65f, glm_rad(0 * (360.0f / WOF_PARTS) + 180.0f / WOF_PARTS), (vec2s){ .x = 0.6f, .y = 0.6f, });
  wof_append_image(&wof, 1, 0.65f, glm_rad(1 * (360.0f / WOF_PARTS) + 180.0f / WOF_PARTS), (vec2s){ .x = 0.6f, .y = 0.6f, });
  wof_append_image(&wof, 2, 0.65f, glm_rad(2 * (360.0f / WOF_PARTS) + 180.0f / WOF_PARTS), (vec2s){ .x = 0.6f, .y = 0.6f, });
  wof_append_image(&wof, 3, 0.65f, glm_rad(3 * (360.0f / WOF_PARTS) + 180.0f / WOF_PARTS), (vec2s){ .x = 0.6f, .y = 0.6f, });
  wof_append_image(&wof, 4, 0.65f, glm_rad(4 * (360.0f / WOF_PARTS) + 180.0f / WOF_PARTS), (vec2s){ .x = 0.6f, .y = 0.6f, });

  wof_append_static_circle(&wof, 0.25f, wof_border_color);
  wof_append_static_image(
    &wof,
    (vec2s){ .x = 0.3f, .y = 0.3f, },
    (vec4s){ .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f, },
    (vec4s){ .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 0.0f, }
  );

  mesh2d_buffer_data(&wof.wheel_geometry);
  for(size_t i = 0; i < WOF_PARTS; i++){
    mesh2d_buffer_data(&wof.wheel_images[i]);
  }
  mesh2d_buffer_data(&wof.static_geometry);
  mesh2d_buffer_data(&wof.static_image);

  return wof;
}

void
wof_destroy(struct wof *wof){
  mesh2d_destroy(&wof->wheel_geometry);
  mesh2d_destroy(&wof->static_geometry);
  mesh2d_destroy(&wof->static_image);
  for(size_t i = 0; i < WOF_PARTS; i++){
    mesh2d_destroy(&wof->wheel_images[i]);
  }
}

void
wof_update(struct wof *wof, float delta_time){
  switch(wof->state){
  case WOF_STATE_STATIC:
    assert(wof->speed == 0.0f);
    break;
  case WOF_STATE_SPINNING:
    wof->angle += wof->speed * delta_time; 
    wof->speed -= WOF_FRICTION * delta_time;
    if(wof->speed < 0.0f) {
      wof->speed = 0.0f;
      wof->state = WOF_STATE_STATIC;
    }
    break;
  default: assert(0);
  }

  mat4s transform = glms_mat4_identity();
  transform = glms_translate(transform, (vec3s){ .x = RESOLUTION_X / 2.0f, .y = RESOLUTION_Y / 2.0f, });
  wof->static_geometry.model_transform = transform; // apply before rotation
  wof->static_image.model_transform = transform; // apply before rotation

  transform = glms_rotate(transform, wof->angle, (vec3s){ .z = 1.0f, });

  wof->wheel_geometry.model_transform = transform;
  for(size_t i = 0; i < WOF_PARTS; i++){
    wof->wheel_images[i].model_transform = transform;
  }
}

void
wof_try_spin(struct wof *wof, float intensity){
  assert(intensity >= 0.0f && intensity <= 1.0f);
  if(wof->state == WOF_STATE_STATIC){
    wof->speed = WOF_MIN_SPEED + (WOF_MAX_SPEED - WOF_MIN_SPEED) * intensity;
    wof->state = WOF_STATE_SPINNING;
  }
}

void
wof_render(const struct wof *wof){
  mesh2d_render(&wof->wheel_geometry, &shader_mesh2d_plain, (vec2s){ .x = RESOLUTION_X, .y = RESOLUTION_Y, });
  for(size_t i = 0; i < WOF_PARTS; i++){
    mesh2d_render(&wof->wheel_images[i], &shader_mesh2d_index, (vec2s){ .x = RESOLUTION_X, .y = RESOLUTION_Y, });
  }
  mesh2d_render(&wof->static_geometry, &shader_mesh2d_plain, (vec2s){ .x = RESOLUTION_X, .y = RESOLUTION_Y, });
  mesh2d_render(&wof->static_image, &shader_mesh2d_index, (vec2s){ .x = RESOLUTION_X, .y = RESOLUTION_Y, });
}
