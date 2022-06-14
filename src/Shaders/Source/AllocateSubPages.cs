#include "bgfx_compute.sh"
//#include "Allocation.sh"

BUFFER_WR(bufferAddressPages, uint, 0);
IMAGE2D_WR(s_worldToPagesSubLevel, rgba8, 1);
BUFFER_WR(bufferPages, uint, 2);
BUFFER_RW(bufferCounter, uint, 3);
IMAGE2D_RO(s_worldToPageTags, r8, 4);
IMAGE2D_WR(s_worldToPageTagsNext, r8, 5);

void AllocatePage(ivec3 addr, float pageTag)
{
    uint counter;
    atomicFetchAndAdd(bufferCounter[0], 1, counter);

    uint pageAddress = addr.x + (addr.y << 10) + (addr.z << 20);
    uint page = bufferPages[counter];

    //bufferPages[counter] = page;
    bufferAddressPages[counter] = pageAddress;

    imageStore(s_worldToPagesSubLevel, addr.xy, ivec4(page & 0xF, (page >> 4) & 0xF, 0, 0) / 255.);
    imageStore(s_worldToPageTagsNext, addr.xy, vec4(pageTag, 0., 0., 0.)/255.);
}

NUM_THREADS(16, 16, 1)
void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
    ivec3 dx = ivec3(1, 0, 0);
    ivec3 dy = ivec3(0, 1, 0);

    ivec3 addr = coord * 2;

    vec4 tag00 = imageLoad(s_worldToPageTags, addr);
    vec4 tag10 = imageLoad(s_worldToPageTags, addr + dx);
    vec4 tag01 = imageLoad(s_worldToPageTags, addr + dy);
    vec4 tag11 = imageLoad(s_worldToPageTags, addr + dx + dy);

    if (tag00.x > 0. || tag10.x > 0. || tag01.x > 0. || tag11.x > 0.)
    {
        AllocatePage(coord, 1.);
    } else {
        imageStore(s_worldToPageTagsNext, coord.xy, vec4(0., 0., 0., 0.));
    }
}
