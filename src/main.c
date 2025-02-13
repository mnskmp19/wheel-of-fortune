
#include <stdio.h>

#include "input/window.h"
#include "input/callbacks.h"

#include "util/memory_safety.h"
#include "util/utils.h"

#include "game/wof.h"

int main(void){
  _memory_debug_init_();
  
  // init statics from callback header
  window_init();

  // init timekeeping
  double old_time = glfwGetTime();
  srand(old_time); // seed RNG

  static double when_a_second_passes = 0.0f;
  static unsigned frames_per_second = 0;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // glDisable(GL_CULL_FACE);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  wof_assets_init();
  if(!wof_assets_check_if_valid()) goto cleanup;

  struct wof wof = wof_create();

  while(!window_should_close() /*&& !game_should_terminate()*/ ){

    // call input api
    raw_input_reset(); // reset buttons' changed state to 0
    char_input_reset();
    glfwPollEvents();

    // logic
    {
      const struct raw_input *raw_input = raw_input_get();
      if(raw_input->buttons[GLFW_KEY_SPACE].is_changed
          && raw_input->buttons[GLFW_KEY_SPACE].is_down){
        wof_try_spin(&wof, frand());
      }
    }
    wof_update(&wof, delta_time);

    // render
    glClearColor(0.2f, 0.3f, 0.5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    wof_render(&wof);
    
    double new_time = glfwGetTime();
    delta_time = new_time - old_time;
    old_time = new_time;

    when_a_second_passes += delta_time;
    frames_per_second++;

    while(when_a_second_passes > 1.0f){
      char buffer[32] = { 0 };
      snprintf(buffer, 32, "hello - %u fps", frames_per_second);
      if(window_is_fullscreen()) printf("FPS = %u\n", frames_per_second);
      window_set_title(buffer);
      
      when_a_second_passes -= 1.0f;
      frames_per_second = 0;
    }
    
    // CAP DELTA TIME FOR DEBUGING
    if(delta_time > 0.25f) delta_time = 0.25f;

    // call render api
    window_swap_buffers();
  }

  wof_destroy(&wof);

cleanup:
  wof_assets_free();
  
  // terminate glfw
  window_terminate();

  _memory_debug_print_();
  _memory_debug_finish_();
}