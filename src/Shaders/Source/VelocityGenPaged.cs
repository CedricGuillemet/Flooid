#include "bgfx_compute.sh"
#include "Paging.sh"

BUFFER_RO(bufferAddressPages, uint, 1);
BUFFER_RO(bufferPages, uint, 2);

IMAGE2D_RW(s_texOut, rgba16f, 0);

uniform vec4 position; // radius in w
uniform vec4 direction; // xy

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    
    uint page = bufferPages[coord.y / 16];
    uint pageAddress = bufferAddressPages[coord.y / 16];

    ivec3 outBase;
    vec3 voxelWorldPos;
    WorldPosFromPage(page, pageAddress, coord, outBase, voxelWorldPos);


    //float dist = -(length(position.xy - p) - position.w);
    float linDistance = position.w - length(position.xy - voxelWorldPos.xy);


    vec4 velocity = imageLoad(s_texOut, outBase);
    velocity += vec4(0.,30.3,0.,0.) * max(linDistance, 0.);
    imageStore(s_texOut, outBase, velocity);
}
