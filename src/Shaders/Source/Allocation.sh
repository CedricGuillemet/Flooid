#include "bgfx_compute.sh"


BUFFER_RW(bufferCounter, uint, 4);
BUFFER_RW(bufferFreePages, uint, 0);
BUFFER_WR(bufferAddressPages, uint, 1);
IMAGE2D_WR(s_worldToPages, rgba8, 2);
BUFFER_WR(bufferPages, uint, 3);
IMAGE2D_WR(s_worldToPageTags, r8, 5);

void AllocatePage(ivec3 addr, float pageTag)
{
    uint counter;
    atomicFetchAndAdd(bufferCounter[0], 1, counter);

    uint pageAddress = addr.x + (addr.y << 10) + (addr.z << 20);
    uint page = bufferFreePages[counter];

    bufferPages[counter] = page;
    bufferAddressPages[counter] = pageAddress;

    imageStore(s_worldToPages, addr.xy, ivec4(page & 0xF, (page >> 4) & 0xF, 0, 0) / 255.);
    imageStore(s_worldToPageTags, addr.xy, vec4(pageTag, 0, 0, 0)/255.);
}


void FreePage(uint pageIndex)
{
    uint previousAllocatedCount;
    atomicFetchAndAdd(bufferCounter[0], -1, previousAllocatedCount);

    // add newly freed page
    bufferFreePages[previousAllocatedCount-1] = bufferPages[pageIndex];

    // replace it with last allocated page in used pages
    bufferPages[pageIndex] = bufferPages[previousAllocatedCount - 1];
}