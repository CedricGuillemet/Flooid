# include "bgfx_compute.sh"
# include "Paging.sh"

#include "CommonFluid.sh"

BUFFER_RO(bufferAddressTiles, uint, 3);
IMAGE3D_WR(s_advectedOut, rgba32f, 4);
BUFFER_RO(bufferTiles, uint, 5);

#define FetchInTile FetchInTile1
#define FetchInVelocity FetchInTile2

vec4 SampleBilerpTile2D(vec2 coord)
{
	ivec3 icoord = ivec3(floor(coord), 0);
	vec4 t00 = FetchInTile(icoord);
	vec4 t01 = FetchInTile(icoord + ivec3(0, 1, 0));
	vec4 t10 = FetchInTile(icoord + ivec3(1, 0, 0));
	vec4 t11 = FetchInTile(icoord + ivec3(1, 1, 0));

	vec2 t = fract(coord);

	return mix(mix(t00, t10, t.x), mix(t01, t11, t.x), t.y);
}

vec4 SampleBilerpTile3D(vec3 coord)
{
    ivec3 icoord = floor(coord);
    vec4 t000 = FetchInTile(icoord);
    vec4 t010 = FetchInTile(icoord + ivec3(0, 1, 0));
    vec4 t100 = FetchInTile(icoord + ivec3(1, 0, 0));
    vec4 t110 = FetchInTile(icoord + ivec3(1, 1, 0));

    vec4 t001 = FetchInTile(icoord + ivec3(0, 0, 1));
    vec4 t011 = FetchInTile(icoord + ivec3(0, 1, 1));
    vec4 t101 = FetchInTile(icoord + ivec3(1, 0, 1));
    vec4 t111 = FetchInTile(icoord + ivec3(1, 1, 1));

    vec3 t = fract(coord);

    return mix(mix(mix(t000, t100, t.x), mix(t010, t110, t.x), t.y),
    mix(mix(t001, t101, t.x), mix(t011, t111, t.x), t.y), t.z);
}


NUM_THREADS(8, 8, 8)
void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);

	uint tile = bufferTiles[gl_WorkGroupID.y];
	uint tileAddress = bufferAddressTiles[gl_WorkGroupID.y];

	ivec3 invocationCoord = WorldCoordFromTile(tileAddress, ivec3(coord.x & 0xF, coord.y & 0xF, coord.z & 0xF));

	vec4 velocity = FetchInVelocity(invocationCoord);
	vec3 uvAdvected = vec3(invocationCoord.xyz) - velocity.xyz * 0.1;
	vec4 value = SampleBilerpTile3D(uvAdvected);


	ivec3 destOut = GetOutAddr(tile, coord);

	imageStore(s_advectedOut, destOut, value);
}
