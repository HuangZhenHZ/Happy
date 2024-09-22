#version 330 core
out vec4 Out_Color;
in vec2 Frag_UV;
uniform sampler2D Texture;
void main() {
  Out_Color = texture(Texture, Frag_UV);
}
