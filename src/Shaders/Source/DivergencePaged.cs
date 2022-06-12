# include "bgfx_compute.sh"
# include "Paging.sh"
#include "CommonFluid.sh"

BUFFER_RO(bufferAddressPages, uint, 3);
IMAGE2D_WR(s_divergenceOut, rgba32f, 4);
BUFFER_RO(bufferPages, uint, 5);


NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
	uint page = bufferPages[uint(coord.y) / 16];
    uint pageAddress = bufferAddressPages[uint(coord.y) / 16];
	
	ivec3 invocationCoord = WorldCoordFromPage(pageAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));
	
	float wL = FetchInPage1(invocationCoord - dx).x;
	float wR = FetchInPage1(invocationCoord + dx).x;
	float wB = FetchInPage1(invocationCoord - dy).y;
	float wT = FetchInPage1(invocationCoord + dy).y;
	
	float scale = 0.5 / 1.;
	float divergence = scale * (wR - wL + wT - wB);

	ivec3 destOut = ivec3(page&0xF, page>>4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

	imageStore(s_divergenceOut, destOut, vec4(divergence, 0, 0, 1));
}
