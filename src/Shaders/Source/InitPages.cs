# include "bgfx_compute.sh"

BUFFER_WR(bufferCounter, uint, 1);
BUFFER_WR(bufferPages, uint, 0);

uniform vec4 initPageCount;
NUM_THREADS(1, 1, 1)
void main()
{
    bufferCounter[0] = 0;
    uint pageCount = uint(initPageCount.x);
    for (uint i = 0; i < pageCount; i++)
    {
        bufferPages[i] = i + 1;
    }
}
