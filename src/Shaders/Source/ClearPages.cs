# include "bgfx_compute.sh"
# include "Paging.sh"

IMAGE2D_WR(s_texOut, rgba32f, 0);
BUFFER_RO(bufferPages, uint, 1);

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    uint page = bufferPages[uint(coord.y) / 16];

    ivec3 destOut = ivec3(page & 0xF, page >> 4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

    imageStore(s_texOut, destOut, vec4(0., 0., 0., 0.));
}
