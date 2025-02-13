#version 430 core

in vec2 vf_tex;

out vec4 FragColor;

uniform sampler2D diffuse_map;

void main(){
  FragColor = texture(diffuse_map, vf_tex);
}