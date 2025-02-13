#version 430 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec4 fore_color;
layout (location = 3) in vec4 back_color;

out vec2 vf_tex;

uniform mat4 model;

void main(){
  vf_tex = tex;
  gl_Position = model * vec4(pos, 1.0f, 1.0f);
}