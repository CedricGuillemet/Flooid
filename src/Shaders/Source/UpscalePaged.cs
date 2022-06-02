# include "bgfx_compute.sh"
#include "paging.sh"

/*SAMPLER2D(s_texU, 0);
IMAGE2D_RW(s_texOut, rgba32f, 1);

uniform vec4 fineTexSize;
*/

IMAGE2D_RO(s_texWorldToPageLevel1, rgba32f, 0);
IMAGE2D_RO(s_texPagesLevel1, rgba32f, 1);
BUFFER_RO(bufferAddressPages, uint, 2);
BUFFER_RO(bufferPages, uint, 3);
IMAGE2D_RW(s_texOut, rgba32f, 4);

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
    
    ivec3 dx = ivec3(1, 0, 0);
    ivec3 dy = ivec3(0, 1, 0);

    ivec3 invocationCoordLevel1 = invocationCoord / 2;
    vec3 invocationLevel1Fraction = vec3(invocationCoord.x & 1, invocationCoord.y & 1, invocationCoord.z & 1) * 0.5;

    vec4 t00 = FetchInPageLevel1(invocationCoordLevel1);
    vec4 t10 = FetchInPageLevel1(invocationCoordLevel1 + dx);
    vec4 t01 = FetchInPageLevel1(invocationCoordLevel1 + dy);
    vec4 t11 = FetchInPageLevel1(invocationCoordLevel1 + dx + dy);
    
    vec4 v = mix(mix(t00, t10, invocationLevel1Fraction.x), mix(t01, t11, invocationLevel1Fraction.x), invocationLevel1Fraction.y);

    ivec3 destOut = ivec3(page & 0xF, page >> 4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);


    float value = imageLoad(s_texOut, destOut).x;
    value += v;
    

    imageStore(s_texOut, destOut, vec4(value, 0., 0., 0.));
}
