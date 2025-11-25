#version 410

layout(location = 0) in vec4 coord;

uniform mat4 Mvp;     

void main() {
  gl_Position = Mvp*coord;
}
