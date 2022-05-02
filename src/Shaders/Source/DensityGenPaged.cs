# include "bgfx_compute.sh"

BUFFER_WR(bufferAddressPages, uint, 1);
BUFFER_WR(bufferPages, uint, 3);

IMAGE2D_RW(s_texOut, rgba16f, 0);

uniform vec4 position;

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    
    uint page = bufferPages[coord.y / 16];
    uint pageAddress = bufferAddressPages[coord.y / 16];

    ivec3 pagePos = ivec3(pageAddress & 1023, (pageAddress >> 10) & 1023, 0);

    vec3 pageWorldPos = vec3(pagePos);

    // start of page
    ivec3 outBase = ivec3(page&0xF, page>>4, 0) * 16;

    // local offset in page
    vec3 localOffset = vec3(coord.x & 0xF, coord.y & 0xF, 0.);
    outBase += localOffset;

    vec3 voxelWorldPos = pageWorldPos * (1./ 16.) + localOffset * (1./ 256.);


    float linDistance = length(position.xy - voxelWorldPos.xy);
    float value = step(0., position.w - linDistance);

    vec4 color = vec4(value, value, value, 1.);
    
    imageStore(s_texOut, outBase, color);
}
