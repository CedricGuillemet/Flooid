# include "bgfx_compute.sh"

BUFFER_WR(bufferPages, uint, 0);
BUFFER_RW(bufferCounter, uint, 1);
BUFFER_WR(bufferAddressPages, uint, 2);

BUFFER_RW(bufferActivePages, uint, 3);
BUFFER_RW(bufferFreedPages, uint, 4);
BUFFER_RW(bufferActivePageAdresses, uint, 5);

NUM_THREADS(1, 1, 1)
void main()
{
    // active page count
    uint activePageCount = bufferCounter[1];
    bufferCounter[0] = activePageCount;
    for (int i = 0; i < bufferCounter[0]; i++)
    {
        bufferPages[i] = bufferActivePages[i];
        bufferAddressPages[i] = bufferActivePageAdresses[i];
    }
    // free pages
    for (int i = 0; i < bufferCounter[2]; i++)
    {
        bufferPages[activePageCount + i] = bufferFreedPages[i];
    }
    bufferCounter[1] = 0;
    bufferCounter[2] = 0;
}
