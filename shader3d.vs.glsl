#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 UV;
out vec2 Frag_UV;
uniform mat4 transform;
void main() {
  gl_Position = transform * vec4(Position, 1.0f);
  Frag_UV = UV;
}
