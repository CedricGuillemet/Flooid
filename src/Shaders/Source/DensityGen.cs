# include "bgfx_compute.sh"

IMAGE2D_RW(s_density, rgba16f, 0);
uniform vec4 position; // radius in w
NUM_THREADS(16, 16, 1)
void main()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

	ivec2 dx = ivec2(1, 0);
	ivec2 dy = ivec2(0, 1);

	vec2 p = coord / 256.;

    float linDistance = length(position.xy - p);
	float dist = -(length(position.xy - p) - position.w);
	//float value = max(dist, 0.) * 10.;
    //float value = mix(1., 0., linDistance/position.w);
    float value = step(0., position.w - linDistance);

	vec4 color = imageLoad(s_density, coord);
	color = max(color, vec4(value, value, value, value));
	imageStore(s_density, coord, color);
}
