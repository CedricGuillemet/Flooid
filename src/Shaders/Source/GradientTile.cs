# include "bgfx_compute.sh"
# include "Paging.sh"
#include "CommonFluid.sh"

BUFFER_RO(bufferAddressTiles, uint, 3);
IMAGE2D_WR(s_gradientOut, rgba32f, 4);
BUFFER_RO(bufferTiles, uint, 5);

NUM_THREADS(16, 16, 1)
void main()
{
	ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
    uint tile = bufferTiles[gl_WorkGroupID.y];
    uint tileAddress = bufferAddressTiles[gl_WorkGroupID.y];

    ivec3 invocationCoord = WorldCoordFromTile(tileAddress, ivec3(coord.x & 0xF, coord.y & 0xF, coord.z & 0xF));

    float pL = FetchInTile1(invocationCoord - DX).x;
    float pR = FetchInTile1(invocationCoord + DX).x;
    float pB = FetchInTile1(invocationCoord - DY).x;
    float pT = FetchInTile1(invocationCoord + DY).x;

    float scale = 0.66;//0.5 / 1.; // 0.5 / gridscale
    vec2 gradient = scale * vec2(pR - pL, pT - pB);

    vec2 wc = FetchInTile2(invocationCoord).xy;
    vec2 value = wc - gradient;


    ivec3 destOut = GetOutAddr(tile, coord);

    imageStore(s_gradientOut, destOut, vec4(value, 0, 1));
}
