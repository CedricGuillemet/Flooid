# include "bgfx_compute.sh"

IMAGE2D_RW(s_velocity, rgba16f, 0);
uniform vec4 position; // radius in w
uniform vec4 direction; // xy
NUM_THREADS(16, 16, 1)
void main()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

	ivec2 dx = ivec2(1, 0);
	ivec2 dy = ivec2(0, 1);

	vec2 p = coord / 256.;

	float dist = -(length(position.xy - p) - position.w);
	vec4 value = direction * max(dist, 0.);

	vec4 color = imageLoad(s_velocity, coord);
	color += value;
	imageStore(s_velocity, coord, color);
}
