#version 410

layout(location = 0) in vec4 coord;
layout(location = 3) in vec2 texcoord;

out data {
  vec3 pos;
  vec2 texcoord;
} v;

void main (void) 
{
  v.pos = coord.xyz;
  v.texcoord = texcoord;
}

