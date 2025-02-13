#include "shader.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

#include "util/memory_safety.h"
#include "util/error_handling.h"
#include "util/generic_impl.h"
#include "camera.h"

#define LOG_BUFFER_SIZE 0x10000
#define STR_BUFFER_SIZE 128

struct shader shader_create_vf_from_file(const char* vert_src_path, const char* frag_src_path){
  struct shader shader = shader_create_empty();

  shader_attach_from_file(&shader, vert_src_path, SHADER_PART_VERTEX);
  shader_attach_from_file(&shader, frag_src_path, SHADER_PART_FRAGMENT);
  shader_link(&shader);
  printf("\n");

  return shader;
}

struct shader shader_create_empty(void){
  struct shader shader;
  shader.is_valid = true;
  shader.id = glCreateProgram();

  if(shader.id == 0) critical_error(ERROR_OPENGL);

  return shader;
}

static char info_log[LOG_BUFFER_SIZE];

static void read_file_into_string(
  struct strbuff* string_buffer,
  const char* input_file_path
){
  FILE* input_file = fopen(input_file_path, "r");

  char input_char = EOF;
  while((input_char = fgetc(input_file)) != EOF){
    strbuff_push(string_buffer, input_char);
  }

  strbuff_push(string_buffer, 0);
  fclose(input_file);
}

void shader_attach_from_file(struct shader* shader, const char* src_path, enum shader_part shader_part){

  static GLuint gl_shader_types[] = {
    [SHADER_PART_VERTEX]          = GL_VERTEX_SHADER,
    [SHADER_PART_GEOMETRY]        = GL_GEOMETRY_SHADER,
    [SHADER_PART_TESS_CONTROL]    = GL_TESS_CONTROL_SHADER,
    [SHADER_PART_TESS_EVALUATION] = GL_TESS_EVALUATION_SHADER,
    [SHADER_PART_FRAGMENT]        = GL_FRAGMENT_SHADER,
  };

  assert(shader_part < SHADER_PART_COUNT);

  int success;
  GLuint gl_shader_type = gl_shader_types[shader_part];

  GLuint shader_obj = glCreateShader(gl_shader_type);
  struct strbuff string_buffer = strbuff_create_default();
  read_file_into_string(&string_buffer, src_path);
  
  glShaderSource(shader_obj, 1, (const char * const*)&string_buffer.data, NULL);
  glCompileShader(shader_obj);
  glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);

  const char* shader_type_print = NULL;
  switch(gl_shader_type){
    case GL_FRAGMENT_SHADER:
      shader_type_print = "Fragment";
      break;
    case GL_VERTEX_SHADER:
      shader_type_print = "Vertex";
      break;
    case GL_TESS_CONTROL_SHADER:
      shader_type_print = "Tess control";
      break;
    case GL_TESS_EVALUATION_SHADER:
      shader_type_print = "Tess evaluation";
      break;
    case GL_GEOMETRY_SHADER:
      shader_type_print = "Geometry";
      break;
    default:
      assert(0);
      break;
  }
  
  strbuff_destroy(&string_buffer);

  if(success == 0){
    glGetShaderInfoLog(shader_obj, LOG_BUFFER_SIZE, 0, info_log);
    printf("%s shader " PRINT_PATH " is NOT successfully compiled.\n", shader_type_print, src_path);
    printf("LOG: %s", info_log);
    shader->is_valid = false;
    return;
  }
  
  printf("%s shader " PRINT_PATH " is successfully compiled.\n", shader_type_print, src_path);
  fflush(stdout);
  
  glAttachShader(shader->id, shader_obj);
}

void shader_link(struct shader* shader){
  int success;

  glLinkProgram(shader->id);

  unsigned shader_parts[5];
  int shader_parts_count = 0;
  glGetAttachedShaders(shader->id, 5, &shader_parts_count, shader_parts);

  for(int i = 0; i < shader_parts_count; i++){
    glDeleteShader(shader_parts[i]);
  }

  glGetProgramiv(shader->id, GL_LINK_STATUS, &success);
  if(success == 0){
    glGetProgramInfoLog(shader->id, LOG_BUFFER_SIZE, NULL, info_log);
    printf("Shader program is NOT successfully linked.\n");
    printf("%s", info_log);
    shader->is_valid = false;
    return;
  }
  
  printf("Shader program is successfully linked.\n");
  fflush(stdout);

}

void shader_destroy(struct shader* shader){
  glDeleteProgram(shader->id);
}

void shader_set_uniform_int(const struct shader* shader, const char* name, int data){
  glUseProgram(shader->id);
  GLint location = glGetUniformLocation(shader->id, name);
  glUniform1i(location, data);
  glUseProgram(0);
}

void shader_set_uniform_float(const struct shader* shader, const char* name, float data){
  glUseProgram(shader->id);
  GLint location = glGetUniformLocation(shader->id, name);
  glUniform1f(location, data);
  glUseProgram(0);
}

void shader_set_uniform_vec3f(const struct shader* shader, const char* name, vec3s data){
  glUseProgram(shader->id);
  GLint location = glGetUniformLocation(shader->id, name);
  glUniform3fv(location, 1, (const float*)data.raw);
  glUseProgram(0);
}

void shader_set_uniform_vec4f(const struct shader* shader, const char* name, vec4s data){
  glUseProgram(shader->id);
  GLint location = glGetUniformLocation(shader->id, name);
  glUniform4fv(location, 1, (const float*)data.raw);
  glUseProgram(0);
}

void shader_set_uniform_mat4f(const struct shader* shader, const char* name, mat4s data){
  glUseProgram(shader->id);
  GLint location = glGetUniformLocation(shader->id, name);
  glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)data.raw); // GL_FALSE because of column-major order
  glUseProgram(0);
}

void shader_set_uniform_mat4f_array(const struct shader* shader, const char* name, const mat4s* data, size_t size){
  assert(size <= 100); // because of the string buffer
  glUseProgram(shader->id);

  size_t indexed_name_len = strlen(name) + strlen("[00]") + 1;
  char* indexed_name = MALLOC(indexed_name_len);
	if (indexed_name == NULL) critical_error(ERROR_MEMORY);

  for(size_t i = 0; i < size; i++){
    snprintf(indexed_name, indexed_name_len, "%s[%"PRIu64"]", name, i);
    GLint location = glGetUniformLocation(shader->id, indexed_name);
    glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)data[i].raw); // GL_FALSE because of column-major order
  }

  FREE(indexed_name);

  // glUniformMatrix4fv(location, size, GL_FALSE, (const float*)data[0].raw); // GL_FALSE because of column-major order
  glUseProgram(0);
}