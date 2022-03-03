# include "bgfx_compute.sh"

SAMPLER2D(s_texVelocity, 0);
SAMPLER2D(s_texVorticity, 1);
IMAGE2D_WR(s_vorticityForceOut, rg16f, 2);

uniform vec4 epsilon;
uniform vec4 curl;

NUM_THREADS(16, 16, 1)
void main()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

	ivec2 dx = ivec2(1, 0);
	ivec2 dy = ivec2(0, 1);

	float wL = texelFetch(s_texVorticity, coord - dx, 0).x;
	float wR = texelFetch(s_texVorticity, coord + dx, 0).x;
	float wB = texelFetch(s_texVorticity, coord - dy, 0).x;
	float wT = texelFetch(s_texVorticity, coord + dy, 0).x;
    float wC = texelFetch(s_texVorticity, coord, 0).x;

	float scale = 0.5 / 1.; // 0.5 / gridscale
	vec2 force = scale * vec2(abs(wT) - abs(wB), abs(wR) - abs(wL));
    float lengthSquared = max(epsilon.x, dot(force, force));
    force *= inversesqrt(lengthSquared) * curl.xy * wC;
    force.y *= -1.0;
    vec2 velc = texelFetch(s_texVelocity, coord, 0).xy;
	vec2 value = velc + (/*timestep * */ force);
	imageStore(s_vorticityForceOut, coord, vec4(value, 0, 1));
}
