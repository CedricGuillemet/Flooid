#include "bgfx_compute.sh"
#include "Allocation.sh"

uniform vec4 groupMin;//, worldMax;

NUM_THREADS(1, 1, 1)
void main()
{
    ivec3 groupMini = ivec3(groupMin.xyz);

    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
    
    ivec3 addr = groupMini + coord;

    ivec4 tag = imageLoad(s_worldToTileTags, addr);
    if (tag.x == 0)
    {
        AllocateTile(addr, 2.);
    }
}
