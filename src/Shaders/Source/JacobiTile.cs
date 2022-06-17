# include "bgfx_compute.sh"
# include "Paging.sh"
#include "CommonFluid.sh"

BUFFER_RO(bufferAddressTiles, uint, 3);
BUFFER_RO(bufferTiles, uint, 4);
IMAGE2D_WR(s_Out, rgba32f, 5);

#define FetchInTileU FetchInTile1
#define FetchInTileRHS FetchInTile2

uniform vec4 jacobiParameters;//alpha, beta

NUM_THREADS(16, 16, 1)
void main()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    uint tile = bufferTiles[gl_WorkGroupID.y];
    uint tileAddress = bufferAddressTiles[gl_WorkGroupID.y];
    
    ivec3 invocationCoord = WorldCoordFromTile(tileAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));

	float omega = 4.f / 5.f;
	
    vec4 u = FetchInTileU(invocationCoord);
	vec4 value = u + omega * 0.25f * (-jacobiParameters.x * FetchInTileRHS(invocationCoord)
		+ FetchInTileU(invocationCoord - DX) +
		+FetchInTileU(invocationCoord + DX) +
		+FetchInTileU(invocationCoord - DY) +
		+FetchInTileU(invocationCoord + DY)
		- 4.f * u
	);
    
    ivec3 destOut = ivec3(tile&0xF, tile>>4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);
 
	imageStore(s_Out, destOut, value);
}
