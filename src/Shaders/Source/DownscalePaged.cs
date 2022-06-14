# include "bgfx_compute.sh"
# include "Paging.sh"
#include "CommonFluid.sh"

IMAGE2D_WR(s_residualDownscaledPagesOut, rgba32f, 3);
BUFFER_RO(bufferAddressPagesNextLevel, uint, 4);
BUFFER_RO(bufferPagesNextLevel, uint, 5);

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    uint pageOut = bufferPagesNextLevel[gl_WorkGroupID.y];
    uint pageAddress = bufferAddressPagesNextLevel[gl_WorkGroupID.y];

    ivec3 invocationCoord = WorldCoordFromPage(pageAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));


    ivec3 coordr = invocationCoord * 2;

    vec4 uc = 0.25 * FetchInPage1(coordr) +
        0.125 * (
        FetchInPage1(coordr - dx)
        + FetchInPage1(coordr + dx)
        + FetchInPage1(coordr - dy)
        + FetchInPage1(coordr + dy)) +
        0.125 * 0.5 * (
        FetchInPage1(coordr - dx - dy)
        + FetchInPage1(coordr + dx - dy)
        + FetchInPage1(coordr - dx + dy)
        + FetchInPage1(coordr + dx + dy));


    ivec3 destOut = ivec3(pageOut & 0xF, pageOut >> 4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

    imageStore(s_residualDownscaledPagesOut, destOut, uc);
}
