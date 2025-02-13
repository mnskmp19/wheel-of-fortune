#include "camera.h"

#include <assert.h>
#include "gfx/gfx_utils.h"

struct camera camera_create_persp_fixed(vec3s pos, vec3s dir, float zoom, float fovx, float aspect, float near, float far){
  struct camera camera;

  float angle = M_PI_2 - glms_vec3_angle(dir, (vec3s){ .x = 0.0f, .y = 0.0f, .z = 1.0f }); // angle with up
  assert(angle <= CAMERA_MAX_PITCH && angle >= -CAMERA_MAX_PITCH);
  assert(zoom >= CAMERA_MIN_ZOOM && zoom <= CAMERA_MAX_ZOOM);
  assert(fovx > 0.0f);

  camera.pos = pos;

  camera.dir = dir;

  camera.up = (vec3s){ .x = 0.0f, .y = 0.0f, .z = 1.0f };

  camera.right = glms_cross(camera.dir, camera.up);
  
  camera.view = glms_look(camera.pos, camera.dir, camera.up);

  camera.type = CAMERA_TYPE_PERSP_FIXED;
  
  camera.persp.zoom  = zoom;

  camera.persp.fovx   = fovx;
  camera.persp.aspect = aspect;
  camera.persp.near   = near;
  camera.persp.far    = far;
  
  // fovx is default field-of-view, real_fovy is field-of-view with zoom
  float real_fovy = 2.0f * atan(tan(camera.persp.fovx / 2.0f) / camera.persp.aspect / camera.persp.zoom);
  camera.proj = glms_perspective(real_fovy, camera.persp.aspect, camera.persp.near, camera.persp.far);

  return camera;
}

struct camera camera_create_ortho_fixed(vec3s pos, vec3s dir, float zoom, float width, float aspect, float near, float far){
  struct camera camera;

  float angle = M_PI_2 - glms_vec3_angle(dir, (vec3s){ .x = 0.0f, .y = 0.0f, .z = 1.0f }); // angle with up
  assert(angle <= CAMERA_MAX_PITCH && angle >= -CAMERA_MAX_PITCH);
  assert(zoom >= CAMERA_MIN_ZOOM && zoom <= CAMERA_MAX_ZOOM);
  assert(width > 0.0f);

  camera.pos = pos;

  camera.dir = dir;

  camera.up = (vec3s){ .x = 0.0f, .y = 0.0f, .z = 1.0f };

  camera.right = glms_cross(camera.dir, camera.up);
  
  camera.view = glms_look(camera.pos, camera.dir, camera.up);

  camera.type = CAMERA_TYPE_ORTHO_FIXED;
  
  camera.ortho.zoom  = zoom;

  camera.ortho.width  = width;
  camera.ortho.aspect = aspect;
  camera.ortho.near   = near;
  camera.ortho.far    = far;
  
  // fovx is default field-of-view, real_fovy is field-of-view with zoom
  // float real_fovy = 2.0f * atan(tan(camera.persp.fovx / 2.0f) / camera.persp.aspect / camera.persp.zoom);
  // camera.proj = glms_perspective(real_fovy, camera.persp.aspect, camera.persp.near, camera.persp.far);

  float real_width = camera.ortho.width / camera.ortho.zoom;
  float real_height = real_width / camera.ortho.aspect;
  camera.proj = glms_ortho(
    -real_width / 2.0f, +real_width / 2.0f,
    -real_height / 2.0f, +real_height / 2.0f,
    camera.ortho.near, camera.ortho.far
  );

  return camera;
}

struct camera camera_create_default(void){
  return camera_create_persp_fixed(
    (vec3s){ .x = 0.0f, .y = 0.0f, .z = 0.0f }, 
    CAMERA_INITIAL_DIR,
    1.0f, CAMERA_DEFAULT_FOV, ASPECT_RATIO, 0.001f, 1000.0f
  );
}

void camera_set_position(struct camera* camera, vec3s pos){
  assert(camera->type == CAMERA_TYPE_PERSP_FIXED || camera->type == CAMERA_TYPE_ORTHO_FIXED
    || camera->type == CAMERA_TYPE_PERSP_UNFIXED || camera->type == CAMERA_TYPE_ORTHO_UNFIXED);

  camera->pos = pos;
  camera->view = glms_look(camera->pos, camera->dir, camera->up);
}

void camera_move_in_direction(struct camera* camera, vec3s dir){
  assert(camera->type == CAMERA_TYPE_PERSP_FIXED || camera->type == CAMERA_TYPE_ORTHO_FIXED
    || camera->type == CAMERA_TYPE_PERSP_UNFIXED || camera->type == CAMERA_TYPE_ORTHO_UNFIXED);

  camera->pos = glms_vec3_add(camera->pos, dir);
  camera->view = glms_look(camera->pos, camera->dir, camera->up);
}

void camera_move_forward(struct camera* camera, float dist){
  assert(camera->type == CAMERA_TYPE_PERSP_FIXED || camera->type == CAMERA_TYPE_ORTHO_FIXED);

  vec3s dir = { .x = camera->dir.x, .y = camera->dir.y, .z = 0.0f };
  dir = glms_normalize(dir);

  camera_move_in_direction(camera, glms_vec3_scale(dir, dist));
}

void camera_move_right(struct camera* camera, float dist){
  assert(camera->type == CAMERA_TYPE_PERSP_FIXED || camera->type == CAMERA_TYPE_ORTHO_FIXED);

  vec3s right = { .x = camera->right.x, .y = camera->right.y, .z = 0.0f };
  right = glms_normalize(right);

  camera_move_in_direction(camera, glms_vec3_scale(right, dist));
}

void camera_move_up(struct camera* camera, float dist){
  assert(camera->type == CAMERA_TYPE_PERSP_FIXED || camera->type == CAMERA_TYPE_ORTHO_FIXED);

  camera_move_in_direction(camera, glms_vec3_scale(camera->up, dist));
}

void camera_set_direction(struct camera* camera, vec3s dir){
  assert(camera->type == CAMERA_TYPE_PERSP_FIXED || camera->type == CAMERA_TYPE_ORTHO_FIXED);

  float angle = M_PI_2 - glms_vec3_angle(dir, camera->up);

  if(angle < -CAMERA_MAX_PITCH) angle = -CAMERA_MAX_PITCH;
  if(angle > +CAMERA_MAX_PITCH) angle = +CAMERA_MAX_PITCH;

  camera->right = glms_vec3_normalize(glms_vec3_cross(dir, camera->up));
  if(glms_vec3_norm(camera->right) == 0.0f) camera->right = (vec3s){ .x = 1.0f, };

  camera->dir = glms_normalize(
    glms_vec3_rotate(
      glms_vec3_cross(camera->up, camera->right),
      angle, 
      camera->right
    )
  );

  camera->view = glms_look(camera->pos, camera->dir, camera->up);
}

void camera_rotate_yaw(struct camera* camera, float delta_angle){
  assert(camera->type == CAMERA_TYPE_PERSP_FIXED || camera->type == CAMERA_TYPE_ORTHO_FIXED);

  camera->dir = glms_normalize(glms_vec3_rotate(camera->dir, delta_angle, camera->up));
  camera->right = glms_normalize(glms_vec3_cross(camera->dir, camera->up));

  camera->view = glms_look(camera->pos, camera->dir, camera->up);
}

void camera_rotate_pitch(struct camera* camera, float delta_angle){
  assert(camera->type == CAMERA_TYPE_PERSP_FIXED || camera->type == CAMERA_TYPE_ORTHO_FIXED);

  float starting_angle = M_PI_2 - glms_vec3_angle(camera->dir, camera->up);

  if(starting_angle + delta_angle < -CAMERA_MAX_PITCH) delta_angle = -CAMERA_MAX_PITCH - starting_angle;
  if(starting_angle + delta_angle > +CAMERA_MAX_PITCH) delta_angle = +CAMERA_MAX_PITCH - starting_angle;
  
  camera->dir = glms_normalize(glms_vec3_rotate(camera->dir, delta_angle, camera->right));

  camera->view = glms_look(camera->pos, camera->dir, camera->up);
}

float camera_get_yaw(const struct camera* camera){
  assert(camera->type == CAMERA_TYPE_PERSP_FIXED || camera->type == CAMERA_TYPE_ORTHO_FIXED);
  return glms_vec3_angle((vec3s){ .x = camera->dir.x, .y = camera->dir.y, }, CAMERA_INITIAL_DIR);
}

float camera_get_pitch(const struct camera* camera){
  assert(camera->type == CAMERA_TYPE_PERSP_FIXED || camera->type == CAMERA_TYPE_ORTHO_FIXED);
  return M_PI_2 - glms_vec3_angle(camera->dir, camera->up);
}

void camera_increase_zoom(struct camera* camera, float inc){

  if(camera->type == CAMERA_TYPE_PERSP_FIXED || camera->type == CAMERA_TYPE_PERSP_UNFIXED){

    camera->persp.zoom *= inc;
    if(camera->persp.zoom < CAMERA_MIN_ZOOM) camera->persp.zoom = CAMERA_MIN_ZOOM;
    if(camera->persp.zoom > CAMERA_MAX_ZOOM) camera->persp.zoom = CAMERA_MAX_ZOOM;

    float real_fovy = 2.0f * atan(tan(camera->persp.fovx / 2.0f) / camera->persp.aspect / camera->persp.zoom);
    camera->proj = glms_perspective(real_fovy, camera->persp.aspect, camera->persp.near, camera->persp.far);

  }
  else if(camera->type == CAMERA_TYPE_ORTHO_FIXED || camera->type == CAMERA_TYPE_ORTHO_UNFIXED){

    camera->ortho.zoom *= inc;
    if(camera->ortho.zoom < CAMERA_MIN_ZOOM) camera->ortho.zoom = CAMERA_MIN_ZOOM;
    if(camera->ortho.zoom > CAMERA_MAX_ZOOM) camera->ortho.zoom = CAMERA_MAX_ZOOM;

    float real_width = camera->ortho.width / camera->ortho.zoom;
    float real_height = real_width / camera->ortho.aspect;
    camera->proj = glms_ortho(
      -real_width / 2.0f, +real_width / 2.0f,
      -real_height / 2.0f, +real_height / 2.0f,
      camera->ortho.near, camera->ortho.far
    );
  }
  else assert(0);
}

// up vector ISN'T fixed
void camera_turn_to_nonfixed(struct camera* camera, vec3s pos, vec3s dir, vec3s up){
  assert(camera->type == CAMERA_TYPE_PERSP_FIXED
    || camera->type == CAMERA_TYPE_PERSP_UNFIXED
    || camera->type == CAMERA_TYPE_ORTHO_FIXED
    || camera->type == CAMERA_TYPE_ORTHO_UNFIXED);
  assert(glms_vec3_norm(glms_vec3_cross(dir, up)) > 0.0f);

  camera->pos = pos;
  camera->dir = glms_vec3_normalize(dir);
  camera->up  = glms_vec3_normalize(up);
  camera->right = glms_vec3_normalize(glms_vec3_cross(camera->dir, camera->up));

  camera->view = glms_look(camera->pos, camera->dir, camera->up);
  
  switch(camera->type){
  case CAMERA_TYPE_PERSP_FIXED:
  case CAMERA_TYPE_PERSP_UNFIXED:
    camera->type = CAMERA_TYPE_PERSP_UNFIXED;
    break;
  case CAMERA_TYPE_ORTHO_FIXED:
  case CAMERA_TYPE_ORTHO_UNFIXED:
    camera->type = CAMERA_TYPE_ORTHO_UNFIXED;
    break;
  default: assert(0);
  }
}

void camera_turn_to_fixed(struct camera* camera){
  assert(camera->type == CAMERA_TYPE_PERSP_UNFIXED || camera->type == CAMERA_TYPE_ORTHO_UNFIXED);

  camera->up = (vec3s){ .x = 0.0f, .y = 0.0f, .z = 1.0f, };
  camera->right = glms_vec3_normalize(glms_vec3_cross(camera->dir, camera->up));
  if(glms_vec3_norm(camera->right) == 0.0f) camera->right = (vec3s){ .x = 1.0f, .y = 0.0f, .z = 0.0f, };
  camera->dir = glms_vec3_normalize(glms_vec3_cross(camera->up, camera->right));

  camera->view = glms_look(camera->pos, camera->dir, camera->up);
  
  switch(camera->type){
  case CAMERA_TYPE_PERSP_UNFIXED:
    camera->type = CAMERA_TYPE_PERSP_FIXED;
    break;
  case CAMERA_TYPE_ORTHO_UNFIXED:
    camera->type = CAMERA_TYPE_ORTHO_FIXED;
    break;
  default: assert(0);
  }
}

void camera_debug_print(const struct camera* camera){
  printf("pos:    " PRINT_VEC3 "\n", PASS_VEC3_PARTS(camera->pos));
  printf("dir:    " PRINT_VEC3 "\n", PASS_VEC3_PARTS(camera->dir));
  printf("up:     " PRINT_VEC3 "\n", PASS_VEC3_PARTS(camera->up));
  printf("right:  " PRINT_VEC3 "\n", PASS_VEC3_PARTS(camera->right));
  printf("\n");

  switch(camera->type){
  case CAMERA_TYPE_PERSP_FIXED:
  case CAMERA_TYPE_PERSP_UNFIXED:
    printf("fovx:    " PRINT_SCALAR "\n", camera->persp.fovx  );
    printf("aspect:  " PRINT_SCALAR "\n", camera->persp.aspect);
    printf("near:    " PRINT_SCALAR "\n", camera->persp.near  );
    printf("far:     " PRINT_SCALAR "\n", camera->persp.far   );
    printf("zoom:    " PRINT_SCALAR "\n", camera->persp.zoom  );
    printf("\n");
    break;
  case CAMERA_TYPE_ORTHO_FIXED:
    printf("width    " PRINT_SCALAR "\n", camera->ortho.width );
    printf("aspect   " PRINT_SCALAR "\n", camera->ortho.aspect);
    printf("near:    " PRINT_SCALAR "\n", camera->ortho.near  );
    printf("far:     " PRINT_SCALAR "\n", camera->ortho.far   );
    printf("zoom:    " PRINT_SCALAR "\n", camera->ortho.zoom  );
    printf("\n");
    break;
  default: assert(0);
  }
}