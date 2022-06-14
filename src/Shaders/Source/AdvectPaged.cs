# include "bgfx_compute.sh"
# include "Paging.sh"

#include "CommonFluid.sh"

BUFFER_RO(bufferAddressPages, uint, 3);
IMAGE2D_WR(s_advectedOut, rgba32f, 4);
BUFFER_RO(bufferPages, uint, 5);

#define FetchInPage FetchInPage1
#define FetchInVelocity FetchInPage2

vec4 SampleBilerpPage(vec2 coord)
{
	ivec3 icoord = ivec3(floor(coord), 0);
	vec4 t00 = FetchInPage(icoord);
	vec4 t01 = FetchInPage(icoord + ivec3(0, 1, 0));
	vec4 t10 = FetchInPage(icoord + ivec3(1, 0, 0));
	vec4 t11 = FetchInPage(icoord + ivec3(1, 1, 0));

	float tx = fract(coord.x);
	float ty = fract(coord.y);

	return mix(mix(t00, t10, tx), mix(t01, t11, tx), ty);
}

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

	uint page = bufferPages[gl_WorkGroupID.y];
	uint pageAddress = bufferAddressPages[gl_WorkGroupID.y];

	ivec3 invocationCoord = WorldCoordFromPage(pageAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));

	vec4 velocity = FetchInVelocity(invocationCoord);
	vec2 uvAdvected = vec2(invocationCoord.xy) - velocity.xy * 0.1;
	vec4 value = SampleBilerpPage(uvAdvected);


	ivec3 destOut = ivec3(page&0xF, page>>4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

	imageStore(s_advectedOut, destOut, value);
}
