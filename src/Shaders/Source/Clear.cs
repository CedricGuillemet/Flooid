# include "bgfx_compute.sh"


IMAGE2D_WR(s_clearOut, rgba16f, 2);

NUM_THREADS(16, 16, 1)
void main()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
	imageStore(s_clearOut, coord, vec4(0., 0., 0., 0.));
}