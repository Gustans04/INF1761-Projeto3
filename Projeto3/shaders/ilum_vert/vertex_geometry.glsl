#version 410

layout(location = 0) in vec4 coord;
layout(location = 1) in vec3 normal;
layout(location = 3) in vec2 texcoord;

out data {
  vec3 pos;
  vec3 normal;
  vec2 texcoord;
} v;

void main (void) 
{
  v.pos = coord.xyz;
  v.normal = normal;
  v.texcoord = texcoord;
}

