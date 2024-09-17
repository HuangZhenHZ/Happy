#version 330 core
in vec2 Frag_UV;
in vec4 Frag_Color;
uniform sampler2D Texture;
out vec4 Out_Color;
void main()
{
    /*
    float d = 1.0 / 8192;
    vec4 c0 = texture(Texture, Frag_UV);
    vec4 c1 = texture(Texture, Frag_UV + vec2(0, d));
    vec4 c2 = texture(Texture, Frag_UV + vec2(0, -d));
    vec4 c3 = texture(Texture, Frag_UV + vec2(d, 0));
    vec4 c4 = texture(Texture, Frag_UV + vec2(-d, 0));
    vec4 c = c0 * 2 - (c1 + c2 + c3 + c4) * 0.25;
    */
    // Out_Color = Frag_Color * c;
    /*
    if (gl_FragCoord.x < 400) {
        Out_Color = vec4(1.0, 0.0, 0.0, 1.0);
    } else {
        Out_Color = Frag_Color * texture(Texture, Frag_UV);
    }
    */
    Out_Color = Frag_Color * texture(Texture, Frag_UV);
}
