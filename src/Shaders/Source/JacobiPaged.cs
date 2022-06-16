# include "bgfx_compute.sh"
# include "Paging.sh"
#include "CommonFluid.sh"

BUFFER_RO(bufferAddressPages, uint, 3);
BUFFER_RO(bufferPages, uint, 4);
IMAGE2D_WR(s_Out, rgba32f, 5);

#define FetchInPageU FetchInPage1
#define FetchInPageRHS FetchInPage2

uniform vec4 jacobiParameters;//alpha, beta

NUM_THREADS(16, 16, 1)
void main()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    uint page = bufferPages[gl_WorkGroupID.y];
    uint pageAddress = bufferAddressPages[gl_WorkGroupID.y];
    
    ivec3 invocationCoord = WorldCoordFromPage(pageAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));

	float omega = 4.f / 5.f;
	
    vec4 u = FetchInPageU(invocationCoord);
	vec4 value = u + omega * 0.25f * (-jacobiParameters.x * FetchInPageRHS(invocationCoord)
		+ FetchInPageU(invocationCoord - DX) +
		+ FetchInPageU(invocationCoord + DX) +
		+ FetchInPageU(invocationCoord - DY) +
		+ FetchInPageU(invocationCoord + DY)
		- 4.f * u
	);
    
    ivec3 destOut = ivec3(page&0xF, page>>4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);
 
	imageStore(s_Out, destOut, value);
}
