# include "bgfx_compute.sh"


IMAGE2D_WR(s_clearOut, rgba16f, 0);

NUM_THREADS(16, 16, 1)
void main()
{
	//ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
	imageStore(s_clearOut, gl_GlobalInvocationID.xy, vec4(0., 0., 0., 0.));
}
