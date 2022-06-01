# include "bgfx_compute.sh"
# include "Paging.sh"

IMAGE2D_RO(s_residualPages, rgba32f, 0);
IMAGE2D_RO(s_texWorldToPage, rgba32f, 1);
IMAGE2D_WR(s_residualDownscaledPagesOut, rgba32f, 2);
BUFFER_RO(bufferAddressPagesNextLevel, uint, 3);
BUFFER_RO(bufferPagesNextLevel, uint, 4);


vec4 FetchInPageResidual(ivec3 coord)
{
    vec4 page = imageLoad(s_texWorldToPage, coord.xy / 16);
    ivec2 localCoord = ivec2(coord.x & 0xF, coord.y & 0xF);
    ivec2 pageCoord = ivec2(page.xy * 255.);
    return imageLoad(s_residualPages, pageCoord * 16 + localCoord);
}


NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    uint pageOut = bufferPagesNextLevel[uint(coord.y) / 16];
    uint pageAddress = bufferAddressPagesNextLevel[uint(coord.y) / 16];

    ivec3 invocationCoord = WorldCoordFromPage(pageAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));

    ivec3 dx = ivec3(1, 0, 0);
    ivec3 dy = ivec3(0, 1, 0);

    ivec3 coordr = invocationCoord * 2;

    vec4 uc = 0.25 * FetchInPageResidual(coordr) +
        0.125 * (
          FetchInPageResidual(coordr - dx)
        + FetchInPageResidual(coordr + dx)
        + FetchInPageResidual(coordr - dy)
        + FetchInPageResidual(coordr + dy)) +
        0.125 * 0.5 * (
          FetchInPageResidual(coordr - dx - dy)
        + FetchInPageResidual(coordr + dx - dy)
        + FetchInPageResidual(coordr - dx + dy)
        + FetchInPageResidual(coordr + dx + dy));


    ivec3 destOut = ivec3(pageOut & 0xF, pageOut >> 4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

    imageStore(s_residualDownscaledPagesOut, destOut, uc);
}
