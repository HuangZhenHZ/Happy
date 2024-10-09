#version 330 core
out vec4 Out_Color;
in vec3 Frag_Pos;
in vec2 Frag_UV;
in vec3 Frag_Normal;
uniform sampler2D Texture;
uniform vec3 light_pos;
void main() {
  vec3 norm = normalize(Frag_Normal);
  vec3 light_dir = normalize(light_pos - Frag_Pos);
  float diff = max(dot(norm, light_dir), 0.0);
  vec3 color = texture(Texture, Frag_UV).rgb * diff;
  Out_Color = vec4(color, 1.0);
}
