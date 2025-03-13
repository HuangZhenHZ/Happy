#version 330 core
in vec2 Frag_UV;
in vec4 Frag_Color;
uniform sampler2D Texture;
out vec4 Out_Color;
void main() {
  Out_Color = vec4(texture(Texture, Frag_UV).rgb, 1.0);
}
