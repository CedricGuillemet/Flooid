# include "bgfx_compute.sh"
# include "Paging.sh"
#include "CommonFluid.sh"

BUFFER_RO(bufferPages, uint, 3);
BUFFER_RO(bufferAddressPages, uint, 4);
IMAGE2D_WR(s_residualOut, rgba32f, 5);

#define FetchInPageU FetchInPage1
#define FetchInPageRHS FetchInPage2

uniform vec4 invhsq;

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

    uint page = bufferPages[uint(coord.y) / 16];
    uint pageAddress = bufferAddressPages[uint(coord.y) / 16];

    ivec3 invocationCoord = WorldCoordFromPage(pageAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));

    vec4 res = FetchInPageRHS(invocationCoord) - (
        4. * FetchInPageU(invocationCoord)
        - FetchInPageU(invocationCoord - dx)
        - FetchInPageU(invocationCoord + dx)
        - FetchInPageU(invocationCoord - dy)
        - FetchInPageU(invocationCoord + dy)
        ) * invhsq.x;

    ivec3 destOut = ivec3(page & 0xF, page >> 4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

    imageStore(s_residualOut, destOut, res);
}
