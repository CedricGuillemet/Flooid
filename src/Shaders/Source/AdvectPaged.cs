# include "bgfx_compute.sh"
# include "Paging.sh"
//# include "SamplingPaged.sh"

IMAGE2D_RO(s_texVelocity, rgba32f,  0);
IMAGE2D_RO(s_texPages, rgba32f, 1);
IMAGE2D_RO(s_texWorldToPage, rgba32f,  2);
BUFFER_RO(bufferAddressPages, uint, 3);
IMAGE2D_WR(s_advectedOut, rgba32f, 4);
BUFFER_RO(bufferPages, uint, 5);

vec4 FetchInPageVelocity(ivec3 coord)
{
    vec4 page = imageLoad(s_texWorldToPage, coord.xy / 16);
    ivec2 localCoord = ivec2(coord.x & 0xF, coord.y & 0xF);
    ivec2 pageCoord = ivec2(page.xy * 255.);
    return imageLoad(s_texVelocity, pageCoord * 16 + localCoord);
}

vec4 FetchInPage(ivec3 coord)
{
	vec4 page = imageLoad(s_texWorldToPage, coord.xy / 16);
	ivec2 localCoord = ivec2(coord.x & 0xF, coord.y & 0xF);
	ivec2 pageCoord = ivec2(page.xy * 255.);
	return imageLoad(s_texPages, pageCoord * 16 + localCoord);
}

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

	uint page = bufferPages[uint(coord.y) / 16];
	uint pageAddress = bufferAddressPages[uint(coord.y) / 16];

	ivec3 invocationCoord = WorldCoordFromPage(pageAddress, ivec3(coord.x & 0xF, coord.y & 0xF, 0));

	vec4 velocity = FetchInPageVelocity(invocationCoord);
	vec2 uvAdvected = vec2(invocationCoord.xy) - velocity.xy * 0.1; //coord / 256. - velocity.xy / 256. + 0.5 / 256.;
	vec4 value = SampleBilerpPage(uvAdvected);// * advection.y;

	//vec4 value = FetchInPage(invocationCoord);

	ivec3 destOut = ivec3(page&0xF, page>>4, 0) * 16 + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

	imageStore(s_advectedOut, destOut, value);
}