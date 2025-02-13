#include "gfx/tex_array.h"

#include "util/utils.h"

#include <stb_image.h>

struct tex_array tex_array_create_empty(enum texture_unit unit, enum tex_filtering filtering){
  struct tex_array tex_array = (struct tex_array){
    .width = 0,
    .height = 0,
    .size = 0,
    .bytes = vec_byte_create_default(),
  };
  glGenTextures(1, &tex_array.id);
  if(tex_array.id == 0) critical_error(ERROR_OPENGL);

  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D_ARRAY, tex_array.id);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

  if(filtering == TEX_FILTERING_NONE){
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  else if(filtering == TEX_FILTERING_LINEAR){
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  else if(filtering == TEX_FILTERING_BILINEAR) {
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  else if(filtering == TEX_FILTERING_TRILINEAR){
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  else assert(0 && "Illegal texture filtering value.");

  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

  tex_array.is_valid = true;

  return tex_array;
}

void tex_array_append(struct tex_array* tex_array, const char* input_file_path){
  assert(input_file_path != NULL);
  stbi_set_flip_vertically_on_load(0);

  int width, height, n_channels;

  unsigned char *data = stbi_load(input_file_path, &width, &height, &n_channels, 4);

  if(data == NULL){
    printf("Failed to load image " PRINT_PATH ".\n", input_file_path);
    tex_array->is_valid = false;
  }

  // set picture width and height
  if(tex_array->size == 0){
    tex_array->width = width;
    tex_array->height = height;
    tex_array->n_channels = n_channels;
  }
  // check consistency
  else if(width != tex_array->width || height != tex_array->height){
    printf("Image " PRINT_PATH " does not have the right format for texture array.\n", input_file_path);
    return;
  }

  vec_byte_push_array(&tex_array->bytes, data, width * height * 4);
  tex_array->size++;
  stbi_image_free(data);
}

void tex_array_buffer_data(struct tex_array* tex_array){
  glActiveTexture(GL_TEXTURE0 + tex_array->unit);
  glBindTexture(GL_TEXTURE_2D_ARRAY, tex_array->id);
  glTexImage3D(
    GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, 
    tex_array->width, tex_array->height, tex_array->size, 
    0, GL_RGBA, GL_UNSIGNED_BYTE,
    tex_array->bytes.data
  );
  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

  vec_byte_destroy(&tex_array->bytes);
}

struct tex_array tex_array_create(
  const char* input_file_paths[],
  size_t n_input,
  enum texture_unit unit,
  enum tex_filtering filtering
){
  struct tex_array tex_array = tex_array_create_empty(unit, filtering);
  for(size_t i = 0; i < n_input; i++){
    tex_array_append(&tex_array, input_file_paths[i]);
  }
  tex_array_buffer_data(&tex_array);

  return tex_array;
}

struct tex_array tex_array_create_default(
  const char* input_file_paths[],
  size_t n_input,
  enum texture_unit unit
){
  return tex_array_create(input_file_paths, n_input, unit, TEX_DEFAULT_FILTERING);
}

void tex_array_use(const struct tex_array* tex_array){
  if(tex_array == NULL) {
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
  }
  else{
    glActiveTexture(GL_TEXTURE0 + tex_array->unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, tex_array->id);
  }
}

void tex_array_destroy(struct tex_array* tex_array){
  vec_byte_destroy(&tex_array->bytes);
  glDeleteTextures(1, &tex_array->id);
}