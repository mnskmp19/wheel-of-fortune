#ifndef _GRAPHICS_CAMERA_H_
#define _GRAPHICS_CAMERA_H_

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <cglm/struct.h>

#include "util/utils.h"
#include "input/window.h"

#define CAMERA_INITIAL_DIR ((vec3s){ .x = 0.0f, .y = 1.0f, .z = 0.0f, })
#define CAMERA_DEFAULT_FOV (90.0f * GLM_PIf / 180.0f)

#define CAMERA_MAX_PITCH (M_PI_2 * 0.95f)
#define CAMERA_MIN_ZOOM 1.0f
#define CAMERA_MAX_ZOOM 100.0f

#define CAMERA_FLY_MOVE_SPEED 8.0f
#define CAMERA_FLY_LOOK_X_SENSITIVITY (2.0f * M_PI / 2000.0f)
#define CAMERA_FLY_LOOK_Y_SENSITIVITY (2.0f * M_PI / 2000.0f)

struct camera {
  vec3s pos;
  vec3s dir;
  vec3s up;
  vec3s right;
  
  union {
    struct persp {
      float fovx;
      float aspect;
      float near;
      float far;
      float zoom;
    } persp;
    struct ortho {
      float width;
      float aspect;
      float near;
      float far;
      float zoom;
    } ortho;
  };
  
  mat4s view;
  mat4s proj;

  enum camera_type {
    CAMERA_TYPE_PERSP_FIXED,
    CAMERA_TYPE_PERSP_UNFIXED,
    CAMERA_TYPE_ORTHO_FIXED,
    CAMERA_TYPE_ORTHO_UNFIXED,
  } type;
};

struct camera camera_create_persp_fixed(vec3s pos, vec3s dir, float zoom, float fovx,  float aspect, float near, float far); // fov in radians
struct camera camera_create_ortho_fixed(vec3s pos, vec3s dir, float zoom, float width, float aspect, float near, float far);
struct camera camera_create_default(void);

// up vector is fixed
void camera_set_position(struct camera*, vec3s pos);
void camera_move_in_direction(struct camera*, vec3s dir);
void camera_move_forward(struct camera*, float dist);
void camera_move_right(struct camera*, float dist);
void camera_move_up(struct camera*, float dist);

// up vector is fixed
void camera_set_direction(struct camera*, vec3s dir);
void camera_rotate_yaw(struct camera*, float delta_angle); 
void camera_rotate_pitch(struct camera*, float delta_angle);

float camera_get_yaw(const struct camera*);
float camera_get_pitch(const struct camera*);

void camera_increase_zoom(struct camera*, float inc);

// up vector ISN'T fixed
void camera_turn_to_nonfixed(struct camera* camera, vec3s pos, vec3s dir, vec3s up);
void camera_turn_to_fixed(struct camera* camera);

void camera_debug_print(const struct camera*);

// void camera_fly_mode_update(struct camera* camera, float delta_time);

#endif