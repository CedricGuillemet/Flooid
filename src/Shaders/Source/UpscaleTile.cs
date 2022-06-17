# include "bgfx_compute.sh"
#include "paging.sh"
#include "CommonFluid.sh"

#define FetchInTile FetchInTile1

BUFFER_RO(bufferAddressTiles, uint, 3);
BUFFER_RO(bufferTiles, uint, 4);
IMAGE2D_RW(s_texOut, rgba32f, 5);

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    uint tile = bufferTiles[gl_WorkGroupID.y];
    uint tileAddress = bufferAddressTiles[gl_WorkGroupID.y];

    ivec3 invocationCoord = WorldCoordFromTile(tileAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));
    
    ivec3 invocationCoordLevel1 = invocationCoord / 2;
    vec3 invocationLevel1Fraction = vec3(invocationCoord.x & 1, invocationCoord.y & 1, invocationCoord.z & 1) * 0.5;

    vec4 t00 = FetchInTile(invocationCoordLevel1);
    vec4 t10 = FetchInTile(invocationCoordLevel1 + DX);
    vec4 t01 = FetchInTile(invocationCoordLevel1 + DY);
    vec4 t11 = FetchInTile(invocationCoordLevel1 + DX + DY);
    
    vec4 v = mix(mix(t00, t10, invocationLevel1Fraction.x), mix(t01, t11, invocationLevel1Fraction.x), invocationLevel1Fraction.y);

    ivec3 destOut = ivec3(tile & 0xF, tile >> 4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);


    float value = imageLoad(s_texOut, destOut).x;
    value += v;
    

    imageStore(s_texOut, destOut, vec4(value, 0., 0., 0.));
}
