#ifndef _GAME_WOF_H_
#define _GAME_WOF_H_

#include "gfx/mesh2d.h"

/* assets */
void
wof_assets_init(void);

bool
wof_assets_check_if_valid(void);

void
wof_assets_free(void);

#define WOF_PARTS 5

struct wof {
  struct mesh2d wheel_geometry;
  struct mesh2d wheel_images[WOF_PARTS];
  struct mesh2d static_geometry;
  struct mesh2d static_image;

  enum wof_state {
    WOF_STATE_STATIC,
    WOF_STATE_SPINNING,

    WOF_STATE_COUNT,
  } state;

  float angle; // in rad
  float speed; // angle/second
};

/* wof */

struct wof
wof_create(void);

void
wof_destroy(struct wof *wof);

void
wof_update(struct wof *wof, float delta_time);

void
wof_try_spin(struct wof *wof, float initial_speed);

void
wof_render(const struct wof *wof);

#endif