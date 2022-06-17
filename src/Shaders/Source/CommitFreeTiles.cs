# include "bgfx_compute.sh"

BUFFER_WR(bufferTiles, uint, 0);
BUFFER_RW(bufferCounter, uint, 1);
BUFFER_WR(bufferAddressTiles, uint, 2);

BUFFER_RW(bufferActiveTiles, uint, 3);
BUFFER_RW(bufferFreedTiles, uint, 4);
BUFFER_RW(bufferActiveTileAdresses, uint, 5);

NUM_THREADS(1, 1, 1)
void main()
{
    // active tile count
    uint activeTileCount = bufferCounter[1];
    bufferCounter[0] = activeTileCount;
    for (int i = 0; i < bufferCounter[0]; i++)
    {
        bufferTiles[i] = bufferActiveTiles[i];
        bufferAddressTiles[i] = bufferActiveTileAdresses[i];
    }
    // free tiles
    for (int i = 0; i < bufferCounter[2]; i++)
    {
        bufferTiles[activeTileCount + i] = bufferFreedTiles[i];
    }
    bufferCounter[1] = 0;
    bufferCounter[2] = 0;
}
