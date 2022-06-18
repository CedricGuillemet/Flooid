#include "bgfx_compute.sh"
#include "Paging.sh"

BUFFER_RO(bufferAddressTiles, uint, 1);
BUFFER_RO(bufferTiles, uint, 2);

IMAGE2D_RW(s_texOut, rgba16f, 0);

uniform vec4 position;

NUM_THREADS(16, 16, 1)
void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
    
    uint tile = bufferTiles[gl_WorkGroupID.y];
    uint tileAddress = bufferAddressTiles[gl_WorkGroupID.y];

    ivec3 outBase;
    vec3 voxelWorldPos;
    WorldPosFromTile(tile, tileAddress, coord, outBase, voxelWorldPos);


    float linDistance = length(position.xyz - voxelWorldPos.xyz);
    float value = step(0., position.w - linDistance);


    vec4 density = imageLoad(s_texOut, outBase);
    //density += vec4(value, value, value, 1.);
    density = max(density, vec4(value, value, value, value));
    imageStore(s_texOut, outBase, density);
}
