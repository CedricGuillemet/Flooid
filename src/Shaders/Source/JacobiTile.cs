# include "bgfx_compute.sh"
# include "Paging.sh"
#include "CommonFluid.sh"

BUFFER_RO(bufferAddressTiles, uint, 3);
BUFFER_RO(bufferTiles, uint, 4);
IMAGE3D_WR(s_Out, rgba32f, 5);

#define FetchInTileU FetchInTile1
#define FetchInTileRHS FetchInTile2

uniform vec4 jacobiParameters;//alpha, beta

NUM_THREADS(8, 8, 8)
void main()
{
	ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
    uint tile = bufferTiles[gl_WorkGroupID.y/2];
    uint tileAddress = bufferAddressTiles[gl_WorkGroupID.y/2];
    
    ivec3 invocationCoord = WorldCoordFromTile(tileAddress, ivec3(coord.x & 0xF, coord.y & 0xF, coord.z & 0xF));

	float omega = 4.f / 5.f;
	
    vec4 u = FetchInTileU(invocationCoord);
	vec4 value = u + omega * 0.25f * (-jacobiParameters.x * FetchInTileRHS(invocationCoord)
		+ FetchInTileU(invocationCoord - DX) +
		+FetchInTileU(invocationCoord + DX) +
		+FetchInTileU(invocationCoord - DY) +
		+FetchInTileU(invocationCoord + DY)
		- 4.f * u
	);
    
    ivec3 destOut = GetOutAddr(tile, coord);
 
	imageStore(s_Out, destOut, value);
}
