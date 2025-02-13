#version 430 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec4 fore_color;
layout (location = 3) in vec4 back_color;

out vec2 vf_tex;
out vec4 vf_fore_color;
out vec4 vf_back_color;

uniform mat4 model;

void main(){
  vf_tex = tex;
  vf_fore_color = fore_color;
  vf_back_color = back_color;
  gl_Position = model * vec4(pos, 1.0f, 1.0f);
}