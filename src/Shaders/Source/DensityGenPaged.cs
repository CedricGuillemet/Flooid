# include "bgfx_compute.sh"

BUFFER_WR(bufferAddressPages, uint, 1);
BUFFER_WR(bufferPages, uint, 3);

IMAGE2D_RW(s_texOut, rgba16f, 0);


NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    
    int page = bufferPages[coord.y / 16];
    
    ivec2 outBase = ivec2(page&0xF, page>>4) * 16;

    outBase += vec2(coord.x & 0xF, coord.y & 0xF);
    
    vec4 color = vec4(1., 1., 1., 1.);
    
    imageStore(s_texOut, outBase, color);
}
