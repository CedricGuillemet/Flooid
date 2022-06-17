# include "bgfx_compute.sh"
# include "Paging.sh"
#include "CommonFluid.sh"

BUFFER_RO(bufferAddressTiles, uint, 3);
IMAGE2D_WR(s_divergenceOut, rgba32f, 4);
BUFFER_RO(bufferTiles, uint, 5);

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
	uint tile = bufferTiles[gl_WorkGroupID.y];
    uint tileAddress = bufferAddressTiles[gl_WorkGroupID.y];
	
	ivec3 invocationCoord = WorldCoordFromTile(tileAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));

	float wL = FetchInTile1(invocationCoord - DX).x;
	float wR = FetchInTile1(invocationCoord + DX).x;
	float wB = FetchInTile1(invocationCoord - DY).y;
	float wT = FetchInTile1(invocationCoord + DY).y;
	
	float scale = 0.5 / 1.;
	float divergence = scale * (wR - wL + wT - wB); 
	
	ivec3 destOut = ivec3(tile&0xF, tile>>4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

	imageStore(s_divergenceOut, destOut, vec4(divergence, 0, 0, 1));
}
