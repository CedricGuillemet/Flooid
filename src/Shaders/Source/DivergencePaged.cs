# include "bgfx_compute.sh"
# include "Paging.sh"
# include "SamplingPaged.sh"

BUFFER_RO(bufferAddressPages, uint, 2);
IMAGE2D_WR(s_divergenceOut, rgba32f, 3);

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    uint pageAddress = bufferAddressPages[uint(coord.y) / 16];
	
	ivec3 invocationCoord = WorldCoordFromPage(pageAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));
	
	ivec3 dx = ivec3(1, 0, 0);
	ivec3 dy = ivec3(0, 1, 0);

	float wL = FetchInPage(invocationCoord - dx).x;
	float wR = FetchInPage(invocationCoord + dx).x;
	float wB = FetchInPage(invocationCoord - dy).y;
	float wT = FetchInPage(invocationCoord + dy).y;
	
	float scale = 0.5 / 1.; // 0.5 / gridscale
	float divergence = scale * (wR - wL + wT - wB);

	imageStore(s_divergenceOut, coord, vec4(divergence, 0, 0, 1));
}