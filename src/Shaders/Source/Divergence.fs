$input v_texcoord0

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

SAMPLER2D(s_texVelocity, 0);

void main()
{
    vec2 dx = vec2(1.0 / 256.0, 0.0);
    vec2 dy = vec2(0.0, 1.0 / 256.0);

    float wL = texture2D(s_texVelocity, vUV - dx).x;
    float wR = texture2D(s_texVelocity, vUV + dx).x;
    float wB = texture2D(s_texVelocity, vUV - dy).y;
    float wT = texture2D(s_texVelocity, vUV + dy).y;

    float scale = 0.5 / 1.; // 0.5 / gridscale
    float divergence = scale * (wR - wL + wT - wB);

    gl_FragColor = vec4(divergence, 0.0, 0.0, 1.0);
}
