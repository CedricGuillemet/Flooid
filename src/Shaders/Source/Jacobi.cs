# include "bgfx_compute.sh"

/*
SAMPLER2D(s_texJacobi, 0); // x
SAMPLER2D(s_texDivergence, 1); // b

uniform vec4 jacobiParameters;//alpha, beta

IMAGE2D_WR(s_jacobiOut, r32f, 2);

NUM_THREADS(16, 16, 1)
void main()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

	ivec2 dx = ivec2(1, 0);
	ivec2 dy = ivec2(0, 1);

	float xL = texelFetch(s_texJacobi, coord - dx, 0).x;
	float xR = texelFetch(s_texJacobi, coord + dx, 0).x;
	float xB = texelFetch(s_texJacobi, coord - dy, 0).x;
	float xT = texelFetch(s_texJacobi, coord + dy, 0).x;

	float bC = texelFetch(s_texDivergence, coord, 0).x;

	float value = (xL + xR + xB + xT + jacobiParameters.x * bC) / jacobiParameters.y;
	//gl_FragColor = vec4(, 0., 0., 1.);
	imageStore(s_jacobiOut, coord, vec4(value, 0, 0, 1));
}
*/


SAMPLER2D(s_texU, 0); // x
SAMPLER2D(s_texRHS, 1); // b

uniform vec4 jacobiParameters;//alpha, beta

IMAGE2D_WR(s_Out, r32f, 2);

NUM_THREADS(16, 16, 1)
void main()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

	ivec2 dx = ivec2(1, 0);
	ivec2 dy = ivec2(0, 1);

	float omega = 4.f / 5.f;
	
	vec4 value = texelFetch(s_texU, coord, 0) + omega * 0.25f * (-jacobiParameters.x/*hsq*/ * texelFetch(s_texRHS, coord, 0)
		+ texelFetch(s_texU, coord - dx, 0) +
		+ texelFetch(s_texU, coord + dx, 0) +
		+ texelFetch(s_texU, coord - dy, 0) +
		+ texelFetch(s_texU, coord + dy, 0) 
		- 4.f * texelFetch(s_texU, coord, 0)
	);

	imageStore(s_Out, coord, value);
}