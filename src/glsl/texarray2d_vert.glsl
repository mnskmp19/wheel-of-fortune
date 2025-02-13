#version 430 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;
layout (location = 4) in float tex_idx;

out vec2 vf_tex;
out float vf_tex_idx;

uniform mat4 model;

void main(){
  vf_tex = tex;
  vf_tex_idx = tex_idx;
  gl_Position = model * vec4(pos, 1.0f, 1.0f);
}