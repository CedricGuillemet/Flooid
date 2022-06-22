# include "bgfx_compute.sh"
# include "Paging.sh"
#include "CommonFluid.sh"

BUFFER_RO(bufferAddressTiles, uint, 3);
IMAGE3D_WR(s_gradientOut, rgba32f, 4);
BUFFER_RO(bufferTiles, uint, 5);

NUM_THREADS(8, 8, 8)
void main()
{
	ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
    uint tile = bufferTiles[gl_WorkGroupID.y/2];
    uint tileAddress = bufferAddressTiles[gl_WorkGroupID.y/2];

    ivec3 invocationCoord = WorldCoordFromTile(tileAddress, ivec3(coord.x & 0xF, coord.y & 0xF, coord.z & 0xF));

    float pL = FetchInTile1(invocationCoord - DX).x;
    float pR = FetchInTile1(invocationCoord + DX).x;
    float pB = FetchInTile1(invocationCoord - DY).x;
    float pT = FetchInTile1(invocationCoord + DY).x;
    float pBk = FetchInTile1(invocationCoord - DZ).x;
    float pF = FetchInTile1(invocationCoord + DZ).x;

    float scale = 0.66;//0.5 / 1.; // 0.5 / gridscale
    vec3 gradient = scale * vec3(pR - pL, pT - pB, pF - pBk);

    vec3 wc = FetchInTile2(invocationCoord).xyz;
    vec3 value = wc - gradient;


    ivec3 destOut = GetOutAddr(tile, coord);

    imageStore(s_gradientOut, destOut, vec4(value, 0));
}
