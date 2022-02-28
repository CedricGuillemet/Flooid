$input v_texcoord0

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

SAMPLER2D(s_texVelocity, 0); // x
SAMPLER2D(s_texDensity, 1); // b

uniform vec4 jacobiParameters;//alpha, beta

void main()
{
    vec2 dx = vec2(1.0 / 256.0, 0.0);
    vec2 dy = vec2(0.0, 1.0 / 256.0);

    float xL = texture2D(s_texVelocity, v_texcoord0 - dx).x;
    float xR = texture2D(s_texVelocity, v_texcoord0 + dx).x;
    float xB = texture2D(s_texVelocity, v_texcoord0 - dy).x;
    float xT = texture2D(s_texVelocity, v_texcoord0 + dy).x;

    float bC = texture2D(s_texDensity, v_texcoord0).x;

    gl_FragColor = vec4((xL + xR + xB + xT + jacobiParameters.x * bC) / jacobiParameters.y, 0., 0., 1.);
}
