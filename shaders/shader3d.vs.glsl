#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 UV;
layout (location = 2) in vec3 Normal;
out vec3 Frag_Pos;
out vec2 Frag_UV;
out vec3 Frag_Normal;
uniform mat4 transform;
void main() {
  gl_Position = transform * vec4(Position, 1.0f);
  Frag_Pos = Position;
  Frag_UV = UV;
  Frag_Normal = Normal;
}
