#version 330 core
out vec4 Out_Color;
in vec3 Frag_Rgb;
void main() {
  Out_Color = vec4(Frag_Rgb, 1.0);
}
