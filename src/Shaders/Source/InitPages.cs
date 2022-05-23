# include "bgfx_compute.sh"

BUFFER_WR(bufferCounter, uint, 1);
BUFFER_WR(bufferFreePages, uint, 0);

NUM_THREADS(1, 1, 1)
void main()
{
    bufferCounter[0] = 0;
    for (uint i = 0; i < 255; i++)
    {
        bufferFreePages[i] = i + 1;
    }
}
