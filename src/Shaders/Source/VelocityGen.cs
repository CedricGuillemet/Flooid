# include "bgfx_compute.sh"

IMAGE2D_RW(s_velocity, rgba32f, 0);
uniform vec4 position; // radius in w
uniform vec4 direction; // xy




NUM_THREADS(16, 16, 1)
void main()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

	vec2 p = coord / 256.;
/*
    float sca = 16.;
    float noiseValueT = snoise(p.xy * sca + vec2(0.,0.01));
    float noiseValueB = snoise(p.xy * sca - vec2(0.,0.01));
    float noiseValueR = snoise(p.xy * sca + vec2(0.01, 0.));
    float noiseValueL = snoise(p.xy * sca - vec2(0.01, 0.));
    */
    vec2 derivative = 1.;//vec2(noiseValueR - noiseValueL, noiseValueT - noiseValueB);

    
	float dist = -(length(position.xy - p) - position.w);
    float linDistance = position.w - length(position.xy - p);
    vec4 value = vec4(0.,30.3,0.,0.) * max(linDistance, 0.);

	vec4 color = imageLoad(s_velocity, coord);
	color += value;
	imageStore(s_velocity, coord, color);
}
