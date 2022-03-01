$input v_texcoord0

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

SAMPLER2D(s_texPressure, 0);
SAMPLER2D(s_texVelocity, 1);

void main()
{
    vec2 dx = vec2(1.0 / 256.0, 0.0);
    vec2 dy = vec2(0.0, 1.0 / 256.0);

    float pL = texture2D(s_texPressure, v_texcoord0 - dx).x;
    float pR = texture2D(s_texPressure, v_texcoord0 + dx).x;
    float pB = texture2D(s_texPressure, v_texcoord0 - dy).x;
    float pT = texture2D(s_texPressure, v_texcoord0 + dy).x;

    float scale = 0.5 / 1.; // 0.5 / gridscale
    vec2 gradient = scale * vec2(pR - pL, pT - pB);

    vec2 wc = texture2D(s_texVelocity, v_texcoord0).xy;

    gl_FragColor = vec4(wc - gradient, 0.0, 1.0);
}
