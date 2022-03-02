# include "bgfx_compute.sh"

SAMPLER2D(s_texPressure, 0);
SAMPLER2D(s_texVelocity, 1);

IMAGE2D_WR(s_gradientOut, rg16f, 2);

NUM_THREADS(16, 16, 1)
void main()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

	ivec2 dx = ivec2(1, 0);
	ivec2 dy = ivec2(0, 1);

    float pL = texelFetch(s_texPressure, coord - dx, 0).x;
    float pR = texelFetch(s_texPressure, coord + dx, 0).x;
    float pB = texelFetch(s_texPressure, coord - dy, 0).x;
    float pT = texelFetch(s_texPressure, coord + dy, 0).x;

    float scale = 0.5 / 1.; // 0.5 / gridscale
    vec2 gradient = scale * vec2(pR - pL, pT - pB);

    vec2 wc = texelFetch(s_texVelocity, coord, 0).xy;
    vec2 value = wc - gradient;

    imageStore(s_gradientOut, coord, vec4(value, 0, 1));
}