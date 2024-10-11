#version 330 core
out vec4 Out_Color;
in vec3 Frag_Pos;
in vec2 Frag_UV;
in vec3 Frag_Normal;
uniform sampler2D Texture;
uniform vec3 light_pos;
uniform vec3 view_pos;
void main() {
  vec3 norm = normalize(Frag_Normal);
  vec3 light_dir = normalize(light_pos - Frag_Pos);
  float diff = max(dot(norm, light_dir), 0.0);
  vec3 color = texture(Texture, Frag_UV).rgb * diff;
  Out_Color = vec4(color, 1.0);

  vec3 view_dir = normalize(view_pos - Frag_Pos);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
  Out_Color += 0.5 * vec4(spec, spec, spec, 0.0);
}
