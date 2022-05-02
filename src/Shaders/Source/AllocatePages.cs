# include "bgfx_compute.sh"


BUFFER_RW(bufferCounter, uint, 4);
BUFFER_RO(bufferFreePages, uint, 0);
BUFFER_WR(bufferAddressPages, uint, 1);
IMAGE2D_WR(s_worldToPages, rgba8, 2);
BUFFER_WR(bufferPages, uint, 3);

uniform vec4 groupMin;//, worldMax;

NUM_THREADS(1, 1, 1)
void main()
{
    uint dummy;
    
    uint counter;
    atomicFetchAndAdd(bufferCounter[0], 1, counter);
    ivec3 groupMini = ivec3(groupMin.xyz);

    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
    
    ivec3 addr = groupMini + coord;
    uint pageAddress = addr.x + (addr.y << 10) + (addr.z << 20);
    uint page = bufferFreePages[counter];
    
    bufferPages[counter] = page;
    bufferAddressPages[counter] = pageAddress;


    imageStore(s_worldToPages, addr.xy, ivec4(page&0xFF, (page>>8)&0xFF, 0, 0) / 255.);
}
