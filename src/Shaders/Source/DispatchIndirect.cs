#include "bgfx_compute.sh"

BUFFER_RW(indirectBuffer, uvec4, 0);
BUFFER_RW(bufferCounter, uint, 1);

NUM_THREADS(1, 1, 1)
void main()
{
    dispatchIndirect(indirectBuffer, 0u, 2u, bufferCounter[0] * 2, 2u);
    dispatchIndirect(indirectBuffer, 1u, 1, bufferCounter[0], 1u);
}
