#ifndef _INPUT_WINDOW_H_
#define _INPUT_WINDOW_H_

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <AL\alc.h>

#define DEFAULT_WINDOW_W (800.0f)
#define DEFAULT_WINDOW_H (450.0f)

#define ASPECT_RATIO (DEFAULT_WINDOW_W / DEFAULT_WINDOW_H)

struct window {
  GLFWwindow* glfw_window;
  ALCdevice* alc_device;
  ALCcontext* alc_context;
  
  // width, height
  int w;
  int h;
  // data for maintaining aspect
  int off_x;
  int off_y;
  int eff_w;
  int eff_h;
};

struct window window_create(int width, int height, const char* name, int is_fullscreen);
void window_destroy(struct window*);
void window_resize(struct window*, int width, int height);

#endif