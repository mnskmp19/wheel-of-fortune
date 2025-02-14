#include "callbacks.h"

#include <stdio.h>
#include <assert.h>
#include "util/utils.h"

static struct window window = { 0 };
static struct raw_input raw_input = { 0 };
static struct char_input char_input = { 0 };
double delta_time = 0.0f;

#define CALLBACK_OUTFILE stdout

#define START_IN_FULLSCREEN 0

void
window_init(void){
  window = window_create(DEFAULT_WINDOW_W, DEFAULT_WINDOW_H, "hello", START_IN_FULLSCREEN);
}

void
window_terminate(void){
  window_destroy(&window);
}

const struct window*
window_get(void){
  return &window;
}

bool
window_should_close(void){
  return glfwWindowShouldClose(window.glfw_window);
}

void
window_swap_buffers(void){
  glfwSwapBuffers(window.glfw_window);
}

bool
window_is_fullscreen(void){
  return glfwGetWindowMonitor(window.glfw_window) != NULL;
}

void
window_set_title(const char* title){
  glfwSetWindowTitle(window.glfw_window, title);
}

void
window_set_viewport(float x, float y, float w, float h){
  assert(0.0f <= x && x <= 1.0f);
  assert(0.0f <= y && y <= 1.0f);
  assert(0.0f <= w && w <= 1.0f);
  assert(0.0f <= h && h <= 1.0f);

  glViewport(
    window.off_x + window.eff_w * x,
    window.off_y + window.eff_h * y,
    window.eff_w * w,
    window.eff_h * h
  );
}

void
raw_input_reset(void){
  for(size_t i = 0; i < array_size(raw_input.buttons); i++){
    raw_input.buttons[i].is_changed = false;
  }
  raw_input.cursor_delta_x = 0.0f;
  raw_input.cursor_delta_y = 0.0f;
  raw_input.scroll_delta_x = 0.0f;
  raw_input.scroll_delta_y = 0.0f;
}

const struct raw_input*
raw_input_get(void){
  return &raw_input;
}

void
char_input_reset(void){
  for(size_t i = 0; i < array_size(char_input.keys); i++){
    char_input.keys[i] = false;
  }
}

const struct char_input*
char_input_get(void){
  return &char_input;
}

void
window_size_callback(GLFWwindow* glfw_window, int w, int h){
  (void)glfw_window; // supress warnings
  window_resize(&window, w, h);
  // fprintf(CALLBACK_OUTFILE, "Window resized: %dx%d\n", w, h);
}

void
key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods){
  (void)glfw_window; // supress warnings
  (void)scancode; // supress warnings
  (void)mods; // supress warnings

  #ifdef PRINT_INPUTS
  static const char* action_print[] = {
    [GLFW_RELEASE] = "RELEASE",
    [GLFW_PRESS]   = "PRESS",
    [GLFW_REPEAT]  = "REPEAT",
  };
  
  fprintf(CALLBACK_OUTFILE, "Key pressed: key=%d, scancode=%d, action=%s, mods=%x\n", 
    key, 
    scancode,
    action_print[action],
    mods
  );
  #endif

  if(key < GLFW_KEY_LAST + 1){
    if(action == GLFW_PRESS) {
      raw_input.buttons[key].is_changed = true;
      raw_input.buttons[key].is_down = true;

      // unicode apparently doesn't recognize ENTER AND BACKSPACE
      // add them to char inputs here
      if(key == GLFW_KEY_ENTER){
        char_input.keys['\n'] = true;
      }
      else if(key == GLFW_KEY_BACKSPACE){
        char_input.keys['\b'] = true;
      }
    }
    else if(action == GLFW_RELEASE){
      raw_input.buttons[key].is_changed = true;
      raw_input.buttons[key].is_down = false;
    }
    else if(action == GLFW_REPEAT){

      if(key == GLFW_KEY_ENTER){
        char_input.keys['\n'] = true;
      }
      else if(key == GLFW_KEY_BACKSPACE){
        char_input.keys['\b'] = true;
      }
    }
  }

}

void
char_callback(GLFWwindow* glfw_window, unsigned code){
  (void)glfw_window; // supress warnings
  if(code < 256){
    char_input.keys[code] = true;
  }
}

void
cursor_position_callback(GLFWwindow* glfw_window, double x_pos, double y_pos){
  // cursor statics
  // static float cursor_last_x = DEFAULT_WINDOW_W / 2.0f;
  // static float cursor_last_y = DEFAULT_WINDOW_H / 2.0f;

  raw_input.cursor_delta_x = x_pos - window.w / 2.0f;
  raw_input.cursor_delta_y = y_pos - window.h / 2.0f;

  // for free cursor
  // cursor_last_x = x_pos;
  // cursor_last_y = y_pos;

  // for cursor fixed to center of the screen
  // cursor_last_x = window.w / 2.0f;
  // cursor_last_y = window.h / 2.0f;

  glfwSetCursorPos(glfw_window, window.w / 2.0f, window.h / 2.0f);
}

void
scroll_callback(GLFWwindow* glfw_window, double xoffset, double yoffset){
  (void)glfw_window; // supress warnings
  raw_input.scroll_delta_y = yoffset;
  raw_input.scroll_delta_x = xoffset;

  #ifdef PRINT_INPUTS
  fprintf(CALLBACK_OUTFILE, "Scrolled: (%d,%d)\n", (int)xoffset, (int)yoffset);
  #endif
}

void
mbutton_callback(GLFWwindow* glfw_window, int button, int action, int mods){

  (void)glfw_window; // supress warnings
  (void)mods; // supress warnings
  
  if(action == GLFW_PRESS){
    raw_input.buttons[button].is_down = 1;
    raw_input.buttons[button].is_changed = 1;
  }
  else if(action == GLFW_RELEASE){
    raw_input.buttons[button].is_down = 0;
    raw_input.buttons[button].is_changed = 1;
  }

  #ifdef PRINT_INPUTS
  static const char* action_print[] = {
    [GLFW_RELEASE] = "RELEASE",
    [GLFW_PRESS]   = "PRESS",
    [GLFW_REPEAT]  = "REPEAT",
  };
  
  double cursor_x = 0.0f, cursor_y = 0.0f;
  glfwGetCursorPos(glfw_window, &cursor_x, &cursor_y);
  cursor_y = window.h - cursor_y;
  
  fprintf(CALLBACK_OUTFILE, "Button pressed: button=%d, action=%s, mods=%x, x=%d, y=%d\n",
    button,
    action_print[action],
    mods,
    (int)cursor_x,
    (int)cursor_y
  );
  #endif
}