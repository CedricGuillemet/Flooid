# include "bgfx_compute.sh"
# include "Paging.sh"
#include "CommonFluid.sh"

IMAGE2D_WR(s_residualDownscaledTilesOut, rgba32f, 3);
BUFFER_RO(bufferAddressTilesCoarser, uint, 4);
BUFFER_RO(bufferTilesCoarser, uint, 5);

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    uint tileOut = bufferTilesCoarser[gl_WorkGroupID.y];
    uint tileAddress = bufferAddressTilesCoarser[gl_WorkGroupID.y];

    ivec3 invocationCoord = WorldCoordFromTile(tileAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));


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


    ivec3 destOut = ivec3(tileOut & 0xF, tileOut >> 4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

    imageStore(s_residualDownscaledTilesOut, destOut, uc);
}
