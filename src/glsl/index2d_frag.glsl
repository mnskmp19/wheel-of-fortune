#version 430 core

in vec2 vf_tex;
in vec4 vf_fore_color;
in vec4 vf_back_color;

out vec4 FragColor;

uniform sampler2D diffuse_map;

/* black denotes maximum value */
void main(){
  vec4 val = texture(diffuse_map, vf_tex);
  float sum = (1.0f - max(val.r, max(val.g, val.b)) / 3.0f) * val.a;

  FragColor = vf_fore_color * sum + vf_back_color * (1.0f - sum);
}