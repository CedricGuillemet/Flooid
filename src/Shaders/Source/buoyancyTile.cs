# include "bgfx_compute.sh"
# include "Paging.sh"
#include "CommonFluid.sh"

BUFFER_RO(bufferAddressTiles, uint, 3);
IMAGE3D_RW(s_velocityTiles, rgba32f, 4);
BUFFER_RO(bufferTiles, uint, 5);

NUM_THREADS(8, 8, 8)
void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
    uint tile = bufferTiles[gl_WorkGroupID.y/2];
    uint tileAddress = bufferAddressTiles[gl_WorkGroupID.y/2];
    
    ivec3 invocationCoord = WorldCoordFromTile(tileAddress, ivec3(coord.x & 0xF, coord.y & 0xF, coord.z & 0xF));

    float wL = FetchInTile1(invocationCoord - DX).y;
    float wR = FetchInTile1(invocationCoord + DX).y;
    float wB = FetchInTile1(invocationCoord - DY).y;
    float wT = FetchInTile1(invocationCoord + DY).y;
    float wBk = FetchInTile1(invocationCoord - DZ).y;
    float wF = FetchInTile1(invocationCoord + DZ).y;

    float M = FetchInTile1(invocationCoord).y;
    float scale = 1.0;
    
    
    ivec3 destOut = GetOutAddr(tile, coord);
    
    vec4 velocity = imageLoad(s_velocityTiles, destOut);
    
    
    vec3 buoyancy = vec3(1., 0., 0.) * (wL - M);
    buoyancy += vec3(-1., 0., 0.) * (wR - M);
    buoyancy += vec3(0., 1., 0.) * (wB - M);
    buoyancy += vec3(0.,-1., 0.) * (wT - M);
    buoyancy += vec3(0., 0., 1.) * (wBk - M);
    buoyancy += vec3(0., 0.,-1.) * (wF - M);

    velocity.xyz += buoyancy * scale;
    
    imageStore(s_velocityTiles, destOut, velocity);
}
