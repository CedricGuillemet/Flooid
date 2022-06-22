# include "bgfx_compute.sh"
# include "Paging.sh"
#include "CommonFluid.sh"

BUFFER_RO(bufferTiles, uint, 3);
BUFFER_RO(bufferAddressTiles, uint, 4);
IMAGE3D_WR(s_residualOut, rgba32f, 5);

#define FetchInTileU FetchInTile1
#define FetchInTileRHS FetchInTile2

uniform vec4 invhsq;

NUM_THREADS(8, 8, 8)
void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);

    uint tile = bufferTiles[gl_WorkGroupID.y/2];
    uint tileAddress = bufferAddressTiles[gl_WorkGroupID.y/2];

    ivec3 invocationCoord = WorldCoordFromTile(tileAddress, ivec3(coord.x & 0xF, coord.y & 0xF, coord.z & 0xF));

    vec4 res = FetchInTileRHS(invocationCoord) - (
        6. * FetchInTileU(invocationCoord)
        - FetchInTileU(invocationCoord - DX)
        - FetchInTileU(invocationCoord + DX)
        - FetchInTileU(invocationCoord - DY)
        - FetchInTileU(invocationCoord + DY)
        - FetchInTileU(invocationCoord - DZ)
        - FetchInTileU(invocationCoord + DZ)
        ) * invhsq.x;

    ivec3 destOut = GetOutAddr(tile, coord);

    imageStore(s_residualOut, destOut, res);
}
