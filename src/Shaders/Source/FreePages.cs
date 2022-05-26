#include "bgfx_compute.sh"
//#include "Allocation.sh"
#include "Paging.sh"

//uniform vec4 groupMin;//, worldMax;
BUFFER_RO(bufferAddressPages, uint, 0);
BUFFER_RO(bufferPages, uint, 1);
IMAGE2D_WR(s_worldToPageTags, r8, 2);
IMAGE2D_RO(s_texPages, rgba32f, 3);


NUM_THREADS(1, 1, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    
    uint page = bufferPages[coord.y];
    uint pageAddress = bufferAddressPages[coord.y];

    ivec3 base = ivec3(page&0xF, page>>4, 0) * 16;// + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

    float threshold = 0.01;
    for (int y = 0; y < 16; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            ivec3 addr = base + ivec3(x, y, 0);
            vec4 value = imageLoad(s_texPages, addr.xy);
            if (value.x > threshold)
            {
                return;
            }
        }
    }
    ivec3 tagPosition = PageAddress(pageAddress);
    imageStore(s_worldToPageTags, tagPosition.xy, vec4(0, 0, 0, 0));
}
