# include "bgfx_compute.sh"
# include "Paging.sh"
//# include "SamplingPaged.sh"

IMAGE2D_RO(s_texPages, rgba32f,  0);
IMAGE2D_RO(s_texWorldToPage, rgba32f,  1);
BUFFER_RO(bufferAddressPages, uint, 2);
IMAGE2D_WR(s_divergenceOut, rgba32f, 3);
BUFFER_RO(bufferPages, uint, 4);

vec4 FetchInPage(ivec3 coord)
{
    vec4 page = imageLoad(s_texWorldToPage, coord.xy / 16);
    ivec2 localCoord = ivec2(coord.x & 0xF, coord.y & 0xF);
    ivec2 pageCoord = ivec2(page.xy * 255.);
    return imageLoad(s_texPages, pageCoord * 16 + localCoord);
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

	float wL = FetchInPage(invocationCoord - dx).x;
	float wR = FetchInPage(invocationCoord + dx).x;
	float wB = FetchInPage(invocationCoord - dy).y;
	float wT = FetchInPage(invocationCoord + dy).y;
	
	float scale = 0.5 / 1.; // 0.5 / gridscale
	float divergence = scale * (wR - wL + wT - wB);

	ivec3 destOut = ivec3(page&0xF, page>>4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

	imageStore(s_divergenceOut, destOut, vec4(divergence, 0, 0, 1));
}