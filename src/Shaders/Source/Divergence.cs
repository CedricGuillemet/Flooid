# include "bgfx_compute.sh"

SAMPLER2D(s_texVelocity, 0);
IMAGE2D_WR(s_divergenceOut, r32f, 1);

NUM_THREADS(16, 16, 1)
void main()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

	ivec2 dx = ivec2(1, 0);
	ivec2 dy = ivec2(0, 1);

	float wL = texelFetch(s_texVelocity, coord - dx, 0).x;
	float wR = texelFetch(s_texVelocity, coord + dx, 0).x;
	float wB = texelFetch(s_texVelocity, coord - dy, 0).y;
	float wT = texelFetch(s_texVelocity, coord + dy, 0).y;

	float scale = 0.5 / 1.; // 0.5 / gridscale
	float divergence = scale * (wR - wL + wT - wB);
    //divergence = 1.;
	imageStore(s_divergenceOut, coord, vec4(divergence, 0, 0, 1));
}
