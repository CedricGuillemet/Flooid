# include "bgfx_compute.sh"
# include "Paging.sh"

IMAGE2D_RO(s_residualPages, rgba32f, 0);
IMAGE2D_RO(s_texWorldToPage, rgba32f, 1);
BUFFER_RO(bufferAddressPages, uint, 2);
BUFFER_RO(bufferPages, uint, 3);


IMAGE2D_WR(s_residualDownscaledPagesOut, rgba32f, 4);
BUFFER_RO(bufferAddressPagesNextLevel, uint, 5);
BUFFER_RO(bufferPagesNextLevel, uint, 6);




/*
bgfx::setImage(0, mResidualPages, 0, bgfx::Access::Read);
bgfx::setImage(1, mWorldToPages, 0, bgfx::Access::Read);
bgfx::setBuffer(2, mBufferAddressPages, bgfx::Access::Read);
bgfx::setBuffer(3, mBufferPages, bgfx::Access::Read);
// l1
bgfx::setImage(4, mResidualDownscaledPages, 0, bgfx::Access::Write);
//bgfx::setImage(1, mWorldToPages, 0, bgfx::Access::Read);
bgfx::setBuffer(5, mBufferAddressPagesLevel1, bgfx::Access::Read);
bgfx::setBuffer(6, mBufferPagesLevel1, bgfx::Access::Read);
*/

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    uint page = bufferPages[uint(coord.y) / 16];
    uint pageAddress = bufferAddressPages[uint(coord.y) / 16];

    ivec3 invocationCoord = WorldCoordFromPage(pageAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));

    ivec3 dx = ivec3(1, 0, 0);
    ivec3 dy = ivec3(0, 1, 0);

    vec4 uc = vec4(1., 1., 1., 1.);
    /*ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 coordr = coord * 2;
    ivec2 dx = ivec2(1, 0);
    ivec2 dy = ivec2(0, 1);

    vec4 uc = 0.25 * texelFetch(s_texU, coordr, 0) +
        0.125 * (
          texelFetch(s_texU, coordr - dx, 0)
        + texelFetch(s_texU, coordr + dx, 0)
        + texelFetch(s_texU, coordr - dy, 0)
        + texelFetch(s_texU, coordr + dy, 0)) +
        0.125 * 0.5 * (
          texelFetch(s_texU, coordr - dx - dy, 0)
        + texelFetch(s_texU, coordr + dx - dy, 0)
        + texelFetch(s_texU, coordr - dx + dy, 0)
        + texelFetch(s_texU, coordr + dx + dy, 0));

    imageStore(s_texOut, coord, uc);*/

    ivec3 destOut = ivec3(page & 0xF, page >> 4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

    imageStore(s_residualDownscaledPagesOut, destOut, uc);
}
