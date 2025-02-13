#ifndef _GRAPHICS_CALLBACK_H_
#define _GRAPHICS_CALLBACK_H_

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <stdbool.h>

#include "input/window.h"

struct raw_input {
  float  cursor_delta_x;
  float  cursor_delta_y;
  float  scroll_delta_x;
  float  scroll_delta_y;

  struct button_status{
    bool is_down;
    bool is_changed;
  } buttons[GLFW_KEY_LAST + 1];
}; // used for actual gameplay

struct char_input {
  bool keys[256];
}; // used for typing into console

extern double delta_time;

void
window_init(void);

void
window_terminate(void);

const struct window*
window_get(void);

bool
window_should_close(void);

void
window_swap_buffers(void);

bool
window_is_fullscreen(void);

void
window_set_title(const char* title);

void
window_set_viewport(float x, float y, float w, float h);

void
raw_input_reset(void);
const struct raw_input*
raw_input_get(void);

void
char_input_reset(void);
const struct char_input*
char_input_get(void);

void
window_size_callback(GLFWwindow*, int w, int h);
void
key_callback(GLFWwindow*, int key, int scancode, int action, int mods);
void
char_callback(GLFWwindow*, unsigned code);
void
cursor_position_callback(GLFWwindow*, double x_pos, double y_pos);
void
scroll_callback(GLFWwindow*, double xoffset, double yoffset);
void
mbutton_callback(GLFWwindow*, int button, int action, int mods);

#endif