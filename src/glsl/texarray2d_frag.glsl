#version 430 core

in vec2 vf_tex;
in float vf_tex_idx;

out vec4 FragColor;

uniform sampler2DArray diffuse_map;

void main(){
  FragColor = texture(diffuse_map, vec3(vf_tex, vf_tex_idx));
}