# include "bgfx_compute.sh"
#include "paging.sh"

/*SAMPLER2D(s_texU, 0);
IMAGE2D_RW(s_texOut, rgba32f, 1);

uniform vec4 fineTexSize;
*/

IMAGE2D_RO(s_texWorldToPage, rgba32f, 1);
IMAGE2D_RO(s_texWorldToPageLevel1, rgba32f, 2);
IMAGE2D_RO(s_texPages, rgba32f, 3);
IMAGE2D_RO(s_texPagesLevel1, rgba32f, 4);

BUFFER_RO(bufferAddressPages, uint, 5);
BUFFER_RO(bufferPages, uint, 6);
IMAGE2D_RW(s_texOut, rgba32f, 0);
/*
vec4 FetchInPage(ivec3 coord)
{
    vec4 page = imageLoad(s_texWorldToPage, coord.xy / 16);
    ivec2 localCoord = ivec2(coord.x & 0xF, coord.y & 0xF);
    ivec2 pageCoord = ivec2(page.xy * 255.);
    return imageLoad(s_texOut, pageCoord * 16 + localCoord);
}
*/
vec4 FetchInPageLevel1(ivec3 coord)
{
    vec4 page = imageLoad(s_texWorldToPageLevel1, coord.xy / 16);
    ivec2 localCoord = ivec2(coord.x & 0xF, coord.y & 0xF);
    ivec2 pageCoord = ivec2(page.xy * 255.);
    return imageLoad(s_texPagesLevel1, pageCoord * 16 + localCoord);
}

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    uint page = bufferPages[uint(coord.y) / 16];
    uint pageAddress = bufferAddressPages[uint(coord.y) / 16];

    ivec3 invocationCoord = WorldCoordFromPage(pageAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));

    /*
    vec2 uv = gl_GlobalInvocationID.xy / fineTexSize.x;

    vec4 v = texture2DLod(s_texU, uv + 1.0 / fineTexSize.x, 0);

    vec4 color = imageLoad(s_texOut, coord);
    color += v;
    imageStore(s_texOut, coord, color);
    */

    ivec3 destOut = ivec3(page & 0xF, page >> 4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);


    float value = imageLoad(s_texOut, destOut).x;

    

    imageStore(s_texOut, destOut, vec4(value, 0., 0., 0.));
}
