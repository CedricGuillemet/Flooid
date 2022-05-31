#include "bgfx_compute.sh"


BUFFER_WR(bufferAddressPages, uint, 0);
IMAGE2D_WR(s_worldToPages, rgba8, 1);
BUFFER_WR(bufferPages, uint, 2);
BUFFER_RW(bufferCounter, uint, 3);
IMAGE2D_WR(s_worldToPageTags, r8, 4);

void AllocatePage(ivec3 addr, float pageTag)
{
    uint counter;
    atomicFetchAndAdd(bufferCounter[0], 1, counter);

    uint pageAddress = addr.x + (addr.y << 10) + (addr.z << 20);
    uint page = bufferPages[counter];

    //bufferPages[counter] = page;
    bufferAddressPages[counter] = pageAddress;

    imageStore(s_worldToPages, addr.xy, ivec4(page & 0xF, (page >> 4) & 0xF, 0, 0) / 255.);
    imageStore(s_worldToPageTags, addr.xy, vec4(pageTag, 0, 0, 0)/255.);
}
