#include "bgfx_compute.sh"

BUFFER_RW(indirectBuffer, uvec4, 0);
BUFFER_RW(bufferCounter, uint, 1);

NUM_THREADS(16, 16, 1)
void main()
{
    dispatchIndirect(indirectBuffer, 0u, 1u, bufferCounter[0], 1u);
}
