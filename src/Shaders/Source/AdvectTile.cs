# include "bgfx_compute.sh"
# include "Paging.sh"

#include "CommonFluid.sh"

BUFFER_RO(bufferAddressTiles, uint, 3);
IMAGE2D_WR(s_advectedOut, rgba32f, 4);
BUFFER_RO(bufferTiles, uint, 5);

#define FetchInTile FetchInTile1
#define FetchInVelocity FetchInTile2

vec4 SampleBilerpTile(vec2 coord)
{
	ivec3 icoord = ivec3(floor(coord), 0);
	vec4 t00 = FetchInTile(icoord);
	vec4 t01 = FetchInTile(icoord + ivec3(0, 1, 0));
	vec4 t10 = FetchInTile(icoord + ivec3(1, 0, 0));
	vec4 t11 = FetchInTile(icoord + ivec3(1, 1, 0));

	float tx = fract(coord.x);
	float ty = fract(coord.y);

	return mix(mix(t00, t10, tx), mix(t01, t11, tx), ty);
}

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

	uint tile = bufferTiles[gl_WorkGroupID.y];
	uint tileAddress = bufferAddressTiles[gl_WorkGroupID.y];

	ivec3 invocationCoord = WorldCoordFromTile(tileAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));

	vec4 velocity = FetchInVelocity(invocationCoord);
	vec2 uvAdvected = vec2(invocationCoord.xy) - velocity.xy * 0.1;
	vec4 value = SampleBilerpTile(uvAdvected);


	ivec3 destOut = ivec3(tile&0xF, tile>>4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

	imageStore(s_advectedOut, destOut, value);
}
