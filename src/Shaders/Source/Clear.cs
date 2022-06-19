# include "bgfx_compute.sh"


IMAGE3D_WR(s_clearOut, rgba16f, 0);

NUM_THREADS(8, 8, 8)
void main()
{
	imageStore(s_clearOut, gl_GlobalInvocationID.xyz, vec4(0., 0., 0., 0.));
}
