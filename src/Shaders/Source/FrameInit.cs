# include "bgfx_compute.sh"

BUFFER_RW(bufferCounter, uint, 0);

NUM_THREADS(1, 1, 1)
void main()
{
    bufferCounter[1] = 0;
    bufferCounter[2] = 0;
}
