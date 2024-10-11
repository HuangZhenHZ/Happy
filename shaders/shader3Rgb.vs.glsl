#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Rgb;
out vec3 Frag_Rgb;
uniform mat4 transform;
void main() {
  gl_Position = transform * vec4(Position, 1.0f);
  Frag_Rgb = Rgb;
}
