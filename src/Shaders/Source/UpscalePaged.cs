# include "bgfx_compute.sh"
#include "paging.sh"
#include "CommonFluid.sh"

#define FetchInPage FetchInPage1

BUFFER_RO(bufferAddressPages, uint, 3);
BUFFER_RO(bufferPages, uint, 4);
IMAGE2D_RW(s_texOut, rgba32f, 5);

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    uint page = bufferPages[gl_WorkGroupID.y];
    uint pageAddress = bufferAddressPages[gl_WorkGroupID.y];

    ivec3 invocationCoord = WorldCoordFromPage(pageAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));
    
    ivec3 invocationCoordLevel1 = invocationCoord / 2;
    vec3 invocationLevel1Fraction = vec3(invocationCoord.x & 1, invocationCoord.y & 1, invocationCoord.z & 1) * 0.5;

    vec4 t00 = FetchInPage(invocationCoordLevel1);
    vec4 t10 = FetchInPage(invocationCoordLevel1 + dx);
    vec4 t01 = FetchInPage(invocationCoordLevel1 + dy);
    vec4 t11 = FetchInPage(invocationCoordLevel1 + dx + dy);
    
    vec4 v = mix(mix(t00, t10, invocationLevel1Fraction.x), mix(t01, t11, invocationLevel1Fraction.x), invocationLevel1Fraction.y);

    ivec3 destOut = ivec3(page & 0xF, page >> 4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);


    float value = imageLoad(s_texOut, destOut).x;
    value += v;
    

    imageStore(s_texOut, destOut, vec4(value, 0., 0., 0.));
}
