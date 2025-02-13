#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include "util/error_handling.h"
#include "util/utils.h"

#define ERROR_TEXTURE ((struct texture){ .id = 0 })

struct texture texture_create(
  const char* input_file_path,
  enum texture_unit unit,
  enum tex_filtering filtering
){
  struct texture texture = {
    .is_valid = false,
    .unit = unit,
  };

  assert(input_file_path != NULL);

  glGenTextures(1, &texture.id);
  if(texture.id == 0) critical_error(ERROR_OPENGL);

  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, texture.id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  if(filtering == TEX_FILTERING_NONE){
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  else if(filtering == TEX_FILTERING_LINEAR){
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  else if(filtering == TEX_FILTERING_BILINEAR) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  else if(filtering == TEX_FILTERING_TRILINEAR){
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  else assert(0 && "Illegal texture filtering value.");
  
  stbi_set_flip_vertically_on_load(0);

  unsigned char *data = stbi_load(input_file_path, &texture.width, &texture.height, &texture.n_channels, 4); 

  if(data == NULL){
    printf("Failed to load texture " PRINT_PATH ".\n", input_file_path);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
  }
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  texture.is_valid = true;
  printf("Texture " PRINT_PATH " successfully loaded.\n", input_file_path);

  stbi_image_free(data);

  return texture;
}

struct texture texture_create_default(
  const char* input_file_path,
  enum texture_unit unit
){
  // return texture_create(input_file_path, unit, TEX_FILTERING_TRILINEAR);
  return texture_create(input_file_path, unit, TEX_DEFAULT_FILTERING);
}

struct texture texture_create_from_buffer(
  const void* buffer,
  size_t size,
  enum texture_unit unit,
  enum tex_filtering filtering
){

  struct texture texture = {
    .is_valid = false,
    .unit = unit,
  };

  glGenTextures(1, &texture.id);
  if(texture.id == 0) critical_error(ERROR_OPENGL);

  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, texture.id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  if(filtering == TEX_FILTERING_NONE){
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  else if(filtering == TEX_FILTERING_LINEAR){
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  else if(filtering == TEX_FILTERING_BILINEAR) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  else if(filtering == TEX_FILTERING_TRILINEAR){
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  else assert(0 && "Illegal texture filtering value.");
  
  stbi_set_flip_vertically_on_load(0);

  unsigned char *data = stbi_load_from_memory(buffer, size, &texture.width, &texture.height, &texture.n_channels, 4); 

  if(data == NULL){
    printf("Failed to load texture from a buffer.\n");
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
  }
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  texture.is_valid = true;
  printf("Texture successfully loaded from a buffer.\n");

  stbi_image_free(data);

  return texture;
}

struct texture texture_create_default_from_buffer(
  const void* buffer,
  size_t size,
  enum texture_unit unit
){
  return texture_create_from_buffer(buffer, size, unit, TEX_DEFAULT_FILTERING);
}

void texture_use(const struct texture* texture){
  if(texture == NULL) {
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  else{
    glActiveTexture(GL_TEXTURE0 + texture->unit);
    glBindTexture(GL_TEXTURE_2D, texture->id);
  }
}

void texture_destroy(struct texture* texture){
  glDeleteTextures(1, &texture->id);
}