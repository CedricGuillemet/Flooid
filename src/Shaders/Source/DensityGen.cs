# include "bgfx_compute.sh"

IMAGE3D_RW(s_density, rgba16f, 0);
uniform vec4 position; // radius in w
NUM_THREADS(8, 8, 8)
void main()
{
	ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);

	vec3 p = coord / 256.;

    float linDistance = length(position.xyz - p);
	float dist = -(length(position.xyz - p) - position.w);
	//float value = max(dist, 0.) * 10.;
    //float value = mix(1., 0., linDistance/position.w);
    float value = step(0., position.w - linDistance);

	vec4 color = imageLoad(s_density, coord);
	color = max(color, vec4(value, value, value, value));
	imageStore(s_density, coord, color);
}
