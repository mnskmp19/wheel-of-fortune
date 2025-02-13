#include "input/window.h"

#include "util/error_handling.h"
#include <stdio.h>

#include "input/callbacks.h"

struct window window_create(int width, int height, const char* window_name, int is_fullscreen){
  // glfw stuff
  struct window window = { 0 };

  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  if(is_fullscreen) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);

    printf("FULLSCREEN SIZE: W = %d, H = %d\n", vidmode->width, vidmode->height);

    window.glfw_window = glfwCreateWindow(vidmode->width, vidmode->height, window_name, monitor, NULL);

    glfwMakeContextCurrent(window.glfw_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetInputMode(window.glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetInputMode(window.glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwSetCursorPos(window.glfw_window, vidmode->width / 2.0f, vidmode->height / 2.0f);

    window_resize(&window, vidmode->width, vidmode->height);
  }
  else{
    window.glfw_window = glfwCreateWindow(width, height, window_name, NULL, NULL);

    glfwMakeContextCurrent(window.glfw_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetInputMode(window.glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetInputMode(window.glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwSetCursorPos(window.glfw_window, width / 2.0f, height / 2.0f);

    window_resize(&window, width, height);
  }

  // openal stuff
  window.alc_device = alcOpenDevice(NULL); // open default device
  if(window.alc_device == NULL) critical_error(ERROR_OPENAL);

  window.alc_context = alcCreateContext(window.alc_device, NULL);
  if(window.alc_context == NULL) critical_error(ERROR_OPENAL);

  if(!alcMakeContextCurrent(window.alc_context)) critical_error(ERROR_OPENAL);

  const ALCchar* sdev_name = NULL;
  if(alcIsExtensionPresent(window.alc_device, "ALC_ENUMERATE_ALL_EXT"))
    sdev_name = alcGetString(window.alc_device, ALC_ALL_DEVICES_SPECIFIER);
  if(sdev_name == NULL || alcGetError(window.alc_device) != ALC_NO_ERROR)
    sdev_name = alcGetString(window.alc_device, ALC_DEVICE_SPECIFIER);
  printf("Audio device openned \"%s\"\n", sdev_name);

  // callbacks  
  glfwSetWindowSizeCallback (window.glfw_window, window_size_callback);
  glfwSetKeyCallback        (window.glfw_window, key_callback);
  glfwSetCharCallback       (window.glfw_window, char_callback);
  glfwSetCursorPosCallback  (window.glfw_window, cursor_position_callback);
  glfwSetScrollCallback     (window.glfw_window, scroll_callback);
  glfwSetMouseButtonCallback(window.glfw_window, mbutton_callback);
  
  // return
  return window;
}

void window_destroy(struct window* window){
  glfwDestroyWindow(window->glfw_window);

  if(!alcMakeContextCurrent(NULL)) critical_error(ERROR_OPENAL);

  alcDestroyContext(window->alc_context);
  if(!alcCloseDevice(window->alc_device)) critical_error(ERROR_OPENAL);
  
  // terminate glfw
  glfwTerminate();
}

void window_resize(struct window* window, int width, int height){
  window->w = width;
  window->h = height;

  // clear everything first
  glViewport(0, 0, width, height);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // add stripes left and right
  if(width > ASPECT_RATIO * height){
    window->eff_w = height * ASPECT_RATIO;
    window->eff_h = height;
    window->off_x = (width - window->eff_w) / 2;
    window->off_y = 0;
  }
  // add stripes up and down
  else {
    window->eff_w = width;
    window->eff_h = width / ASPECT_RATIO;
    window->off_x = 0;
    window->off_y = (height - window->eff_h) / 2;
  }

  glViewport(window->off_x, window->off_y, window->eff_w, window->eff_h);
}