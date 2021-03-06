#include "bgfx_compute.sh"
#include "Paging.sh"

BUFFER_RO(bufferAddressTiles, uint, 1);
BUFFER_RO(bufferTiles, uint, 2);

IMAGE3D_RW(s_texOut, rgba16f, 0);

uniform vec4 position;

NUM_THREADS(8, 8, 8)
void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
    
    uint tile = bufferTiles[gl_WorkGroupID.y/2];
    uint tileAddress = bufferAddressTiles[gl_WorkGroupID.y/2];

    ivec3 outBase;
    vec3 voxelWorldPos;
    WorldPosFromTile(tile, tileAddress, coord, outBase, voxelWorldPos);


    float linDistance = length(position.xyz - voxelWorldPos.xyz);
    float value = step(0., position.w - linDistance);

    vec4 density = imageLoad(s_texOut, outBase);
    //density += vec4(value, value, value, 1.);
    density = max(density, vec4(value*0.9, value, value, value));
    
    //density = vec4(1., 1., 1., 1.);
    //density = vec4(voxelWorldPos.xyz, 0.);
    imageStore(s_texOut, outBase, density);
}
