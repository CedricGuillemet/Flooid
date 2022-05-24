# include "bgfx_compute.sh"
# include "Paging.sh"
//# include "SamplingPaged.sh"

IMAGE2D_RO(s_texPressure, rgba32f, 0);
IMAGE2D_RO(s_texVelocity, rgba32f, 1);
IMAGE2D_RO(s_texWorldToPage, rgba32f, 2);
BUFFER_RO(bufferAddressPages, uint, 3);
IMAGE2D_WR(s_gradientOut, rgba32f, 4);
BUFFER_RO(bufferPages, uint, 5);


//SAMPLER2D(s_texPressure, 0);
//SAMPLER2D(s_texVelocity, 1);

//IMAGE2D_WR(s_gradientOut, rgba32f, 2);

vec4 FetchInPagePressure(ivec3 coord)
{
    vec4 page = imageLoad(s_texWorldToPage, coord.xy / 16);
    ivec2 localCoord = ivec2(coord.x & 0xF, coord.y & 0xF);
    ivec2 pageCoord = ivec2(page.xy * 255.);
    return imageLoad(s_texPressure, pageCoord * 16 + localCoord);
}

vec4 FetchInPageVelocity(ivec3 coord)
{
    vec4 page = imageLoad(s_texWorldToPage, coord.xy / 16);
    ivec2 localCoord = ivec2(coord.x & 0xF, coord.y & 0xF);
    ivec2 pageCoord = ivec2(page.xy * 255.);
    return imageLoad(s_texVelocity, pageCoord * 16 + localCoord);
}

NUM_THREADS(16, 16, 1)
void main()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    uint page = bufferPages[uint(coord.y) / 16];
    uint pageAddress = bufferAddressPages[uint(coord.y) / 16];

    ivec3 invocationCoord = WorldCoordFromPage(pageAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));

    ivec3 dx = ivec3(1, 0, 0);
	ivec3 dy = ivec3(0, 1, 0);

    float pL = FetchInPagePressure(invocationCoord - dx).x;
    float pR = FetchInPagePressure(invocationCoord + dx).x;
    float pB = FetchInPagePressure(invocationCoord - dy).x;
    float pT = FetchInPagePressure(invocationCoord + dy).x;

    float scale = 0.5 / 1.; // 0.5 / gridscale
    vec2 gradient = scale * vec2(pR - pL, pT - pB);

    vec2 wc = FetchInPageVelocity(invocationCoord).xy;
    vec2 value = wc - gradient;

    /*if (coord.x == 0 || coord.x == 255 || coord.y == 0 || coord.y == 255) {
        value = vec2(0., 0.);
    }*/

    ivec3 destOut = ivec3(page & 0xF, page >> 4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

    imageStore(s_gradientOut, destOut, vec4(value, 0, 1));
}
