#include "bgfx_compute.sh"
#include "Paging.sh"

BUFFER_RO(bufferAddressTiles, uint, 1);
BUFFER_RO(bufferTiles, uint, 2);

IMAGE3D_RW(s_texOut, rgba16f, 0);

uniform vec4 position; // radius in w
uniform vec4 direction; // xy

NUM_THREADS(8, 8, 8)
void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
    
    uint tile = bufferTiles[gl_WorkGroupID.y];
    uint tileAddress = bufferAddressTiles[gl_WorkGroupID.y];

    ivec3 outBase;
    vec3 voxelWorldPos;
    WorldPosFromTile(tile, tileAddress, coord, outBase, voxelWorldPos);


    //float dist = -(length(position.xy - p) - position.w);
    float linDistance = position.w - length(position.xyz - voxelWorldPos.xyz);


    vec4 velocity = imageLoad(s_texOut, outBase);
    velocity += vec4(0.,30.3,0.,0.) * max(linDistance, 0.);
    imageStore(s_texOut, outBase, velocity);
}
