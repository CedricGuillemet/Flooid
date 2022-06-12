# include "bgfx_compute.sh"
# include "Paging.sh"
#include "CommonFluid.sh"

BUFFER_RO(bufferAddressPages, uint, 3);
IMAGE2D_WR(s_gradientOut, rgba32f, 4);
BUFFER_RO(bufferPages, uint, 5);

NUM_THREADS(16, 16, 1)
void main()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    uint page = bufferPages[uint(coord.y) / 16];
    uint pageAddress = bufferAddressPages[uint(coord.y) / 16];

    ivec3 invocationCoord = WorldCoordFromPage(pageAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));

    float pL = FetchInPage1(invocationCoord - dx).x;
    float pR = FetchInPage1(invocationCoord + dx).x;
    float pB = FetchInPage1(invocationCoord - dy).x;
    float pT = FetchInPage1(invocationCoord + dy).x;

    float scale = 0.5 / 1.; // 0.5 / gridscale
    vec2 gradient = scale * vec2(pR - pL, pT - pB);

    vec2 wc = FetchInPage2(invocationCoord).xy;
    vec2 value = wc - gradient;


    ivec3 destOut = ivec3(page & 0xF, page >> 4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

    imageStore(s_gradientOut, destOut, vec4(value, 0, 1));
}
