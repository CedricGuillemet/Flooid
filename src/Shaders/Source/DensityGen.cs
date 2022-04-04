# include "bgfx_compute.sh"
#include "Common.shader"

IMAGE2D_RW(s_density, rgba16f, 0);
uniform mat4 invWorldMatrix;
uniform vec4 parameters;

/*
 //float dist = -(length(position.xyz - invp) - 1.);
 //float value = max(dist, 0.) * 10.;
 //float value = mix(1., 0., linDistance/position.w);
 // box
 // sphere
 // torus
 
 /*
 
 float sca = 3;
 float ns = (snoise(p.xy * sca) * snoise(p.xz * sca) * snoise(p.yz * sca)+0.2) * 50.;
 sca = 16;
 ns += (snoise(p.xy * sca) * snoise(p.xz * sca) * snoise(p.yz * sca)+0.2) * 10.;
 //ns = 2.;
  */
 /*
NUM_THREADS(8, 8, 8)
void main()
{
	ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
	vec3 p = coord / 256.;
    
    float sca = 8;
    p += vec3(
    snoise(p.xy * sca), snoise(p.xz * sca), snoise(p.yz * sca)
    ) * 0.05;
    vec3 invp = mul(invWorldMatrix, vec4(p, 1.)).xyz;

    
    //float linDistance = length(invp);
    //float value = step(0., 1. - linDistance);
     
    
    float dist = sdTorus(invp, vec2(0.7, 0.4));
    

    float value = step(0., -dist) * 2.;

    // set value
	vec4 color = imageLoad(s_density, coord);
	color = max(color, vec4(value, value, value, value));
	imageStore(s_density, coord, color);
}
  */


NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    vec2 p = coord / 256.;
    
    vec4 position = vec4(0.5, 0.1, 0., 0.1);
    
    float linDistance = length(position.xy - p);
    float dist = -(length(position.xy - p) - position.w);
    //float value = max(dist, 0.) * 10.;
    //float value = mix(1., 0., linDistance/position.w);
    float value = step(0., position.w - linDistance);

    vec4 color = imageLoad(s_density, coord);
    color = max(color, vec4(value, value, value, value));
    imageStore(s_density, coord, color);
}
