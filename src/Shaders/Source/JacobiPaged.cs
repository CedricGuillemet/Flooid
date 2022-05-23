# include "bgfx_compute.sh"
# include "Paging.sh"
IMAGE2D_RO(s_u, rgba32f,  0);
IMAGE2D_RO(s_texWorldToPage, rgba32f,  1);
BUFFER_RO(bufferAddressPages, uint, 2);
IMAGE2D_RO(s_rhs, rgba32f,  3);
BUFFER_RO(bufferPages, uint, 4);
IMAGE2D_WR(s_Out, rgba32f, 5);


uniform vec4 jacobiParameters;//alpha, beta

vec4 FetchInPageU(ivec3 coord)
{
    vec4 page = imageLoad(s_texWorldToPage, coord.xy / 16);
    ivec2 localCoord = ivec2(coord.x & 0xF, coord.y & 0xF);
    ivec2 pageCoord = ivec2(page.xy * 255.);
    return imageLoad(s_u, pageCoord * 16 + localCoord);
}


vec4 FetchInPageRHS(ivec3 coord)
{
    vec4 page = imageLoad(s_texWorldToPage, coord.xy / 16);
    ivec2 localCoord = ivec2(coord.x & 0xF, coord.y & 0xF);
    ivec2 pageCoord = ivec2(page.xy * 255.);
    return imageLoad(s_rhs, pageCoord * 16 + localCoord);
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

	float omega = 4.f / 5.f;
	
    vec4 u = FetchInPageU(invocationCoord);
	vec4 value = u + omega * 0.25f * (-jacobiParameters.x * FetchInPageRHS(invocationCoord)
		+ FetchInPageU(invocationCoord - dx) +
		+ FetchInPageU(invocationCoord + dx) +
		+ FetchInPageU(invocationCoord - dy) +
		+ FetchInPageU(invocationCoord + dy)
		- 4.f * u
	);
    
    ivec3 destOut = ivec3(page&0xF, page>>4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);
 
	imageStore(s_Out, destOut, value);
}
