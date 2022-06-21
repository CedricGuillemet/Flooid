# include "bgfx_compute.sh"
# include "Paging.sh"
#include "CommonFluid.sh"

IMAGE3D_WR(s_residualDownscaledTilesOut, rgba32f, 3);
BUFFER_RO(bufferAddressTilesCoarser, uint, 4);
BUFFER_RO(bufferTilesCoarser, uint, 5);

NUM_THREADS(8, 8, 8)
void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
    uint tile = bufferTilesCoarser[gl_WorkGroupID.y/2];
    uint tileAddress = bufferAddressTilesCoarser[gl_WorkGroupID.y/2];

    ivec3 invocationCoord = WorldCoordFromTile(tileAddress, ivec3(coord.x & 0xF, coord.y & 0xF, coord.z & 0xF));


    ivec3 coordr = invocationCoord * 2;

    vec4 uc = 0.25 * FetchInTile1(coordr) +
        0.125 * (
        FetchInTile1(coordr - DX)
        + FetchInTile1(coordr + DX)
        + FetchInTile1(coordr - DY)
        + FetchInTile1(coordr + DY)) +
        0.125 * 0.5 * (
        FetchInTile1(coordr - DX - DY)
        + FetchInTile1(coordr + DX - DY)
        + FetchInTile1(coordr - DX + DY)
        + FetchInTile1(coordr + DX + DY));


    ivec3 destOut = GetOutAddr(tile, coord);

    imageStore(s_residualDownscaledTilesOut, destOut, uc);
}
