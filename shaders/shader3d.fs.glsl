#version 330 core

out vec4 Out_Color;
in vec3 Frag_Pos;
in vec2 Frag_UV;
in vec3 Frag_Normal;
uniform sampler2D Texture;
uniform vec3 light_pos;
uniform vec3 view_pos;

void main() {
  vec4 texture_color = texture(Texture, Frag_UV);
  if (texture_color.a < 0.01) {
    discard;
  }

  vec3 light_color = vec3(1.0, 1.0, 1.0);
  float kAmbientStrength = 0.5;
  vec3 ambient = kAmbientStrength * light_color;

  vec3 norm = normalize(Frag_Normal);
  vec3 light_dir = normalize(light_pos - Frag_Pos);
  float diff = max(dot(norm, light_dir), 0.0);
  vec3 diffuse = diff * light_color;

  vec3 view_dir = normalize(view_pos - Frag_Pos);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
  float kSpecularStrength = 1.0;
  vec3 specular = kSpecularStrength * spec * light_color;

  Out_Color = vec4((ambient + diffuse + specular) * texture_color.rgb, texture_color.a);
}
