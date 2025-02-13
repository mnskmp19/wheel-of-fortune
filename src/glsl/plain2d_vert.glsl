#version 430 core

layout (location = 0) in vec2 pos;
layout (location = 2) in vec4 color;

out vec4 vf_color;

uniform mat4 model;

void main(){
  vf_color = color;
  gl_Position = model * vec4(pos, 1.0f, 1.0f);
}