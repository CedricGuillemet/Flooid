# include "bgfx_compute.sh"

BUFFER_WR(bufferCounter, uint, 1);
BUFFER_WR(bufferTiles, uint, 0);

uniform vec4 initTileCount;
NUM_THREADS(1, 1, 1)
void main()
{
    bufferCounter[0] = 0;
    uint tileCount = uint(initTileCount.x);
    for (uint i = 0; i < tileCount; i++)
    {
        bufferTiles[i] = i + 1;
    }
}
