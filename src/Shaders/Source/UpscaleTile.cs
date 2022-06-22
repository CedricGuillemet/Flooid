# include "bgfx_compute.sh"
#include "paging.sh"
#include "CommonFluid.sh"

#define FetchInTile FetchInTile1

BUFFER_RO(bufferAddressTiles, uint, 3);
BUFFER_RO(bufferTiles, uint, 4);
IMAGE3D_RW(s_texOut, rgba32f, 5);

NUM_THREADS(8, 8, 8)
void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
    uint tile = bufferTiles[gl_WorkGroupID.y/2];
    uint tileAddress = bufferAddressTiles[gl_WorkGroupID.y/2];

    ivec3 invocationCoord = WorldCoordFromTile(tileAddress, ivec3(coord.x & 0xF, coord.y & 0xF, coord.z & 0xF));
    
    ivec3 invocationCoordLevel1 = invocationCoord / 2;
    vec3 invocationLevel1Fraction = vec3(invocationCoord.x & 1, invocationCoord.y & 1, invocationCoord.z & 1) * 0.5;

    vec4 t000 = FetchInTile(invocationCoordLevel1);
    vec4 t100 = FetchInTile(invocationCoordLevel1 + DX);
    vec4 t010 = FetchInTile(invocationCoordLevel1 + DY);
    vec4 t110 = FetchInTile(invocationCoordLevel1 + DX + DY);

    vec4 t001 = FetchInTile(invocationCoordLevel1 + DZ);
    vec4 t101 = FetchInTile(invocationCoordLevel1 + DX + DZ);
    vec4 t011 = FetchInTile(invocationCoordLevel1 + DY + DZ);
    vec4 t111 = FetchInTile(invocationCoordLevel1 + DX + DY + DZ);

    vec4 v = mix(mix(mix(t000, t100, invocationLevel1Fraction.x), mix(t010, t110, invocationLevel1Fraction.x), invocationLevel1Fraction.y),
                 mix(mix(t001, t101, invocationLevel1Fraction.x), mix(t011, t111, invocationLevel1Fraction.x), invocationLevel1Fraction.y), invocationLevel1Fraction.z);

    ivec3 destOut = GetOutAddr(tile, coord);

    float value = imageLoad(s_texOut, destOut).x;
    value += v;

    imageStore(s_texOut, destOut, vec4(value, 0., 0., 0.));
}
