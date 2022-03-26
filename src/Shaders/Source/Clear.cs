# include "bgfx_compute.sh"


IMAGE3D_WR(s_clearOut, rgba16f, 0);

NUM_THREADS(8, 8, 8)
void main()
{
	ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
	imageStore(s_clearOut, coord, vec4(0., 0., 0., 0.));
}
