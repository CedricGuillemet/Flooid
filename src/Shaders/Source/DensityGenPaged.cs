#include "bgfx_compute.sh"
#include "Paging.sh"

BUFFER_RO(bufferAddressPages, uint, 1);
BUFFER_RO(bufferPages, uint, 3);

IMAGE2D_RW(s_texOut, rgba16f, 0);

uniform vec4 position;

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    
    uint page = bufferPages[coord.y / 16];
    uint pageAddress = bufferAddressPages[coord.y / 16];

    ivec3 outBase;
    vec3 voxelWorldPos;
    WorldPosFromPage(page, pageAddress, coord, outBase, voxelWorldPos);


    float linDistance = length(position.xy - voxelWorldPos.xy);
    float value = step(0., position.w - linDistance);

    vec4 color = vec4(value, value, value, 1.);
    
    imageStore(s_texOut, outBase, color);
}
