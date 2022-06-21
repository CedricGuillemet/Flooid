# include "bgfx_compute.sh"
# include "Paging.sh"
#include "CommonFluid.sh"

BUFFER_RO(bufferAddressTiles, uint, 3);
IMAGE3D_WR(s_divergenceOut, rgba32f, 4);
BUFFER_RO(bufferTiles, uint, 5);

NUM_THREADS(8, 8, 8)
void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
	uint tile = bufferTiles[gl_WorkGroupID.y/2];
    uint tileAddress = bufferAddressTiles[gl_WorkGroupID.y/2];
	
	ivec3 invocationCoord = WorldCoordFromTile(tileAddress, ivec3(coord.x & 0xF, coord.y & 0xF, coord.z & 0xF));

	float wL = FetchInTile1(invocationCoord - DX).x;
	float wR = FetchInTile1(invocationCoord + DX).x;
	float wB = FetchInTile1(invocationCoord - DY).y;
	float wT = FetchInTile1(invocationCoord + DY).y;
	
	float scale = 0.5 / 1.;
	float divergence = scale * (wR - wL + wT - wB); 
	
    ivec3 destOut = GetOutAddr(tile, coord);

	imageStore(s_divergenceOut, destOut, vec4(divergence, 0, 0, 1));
}
