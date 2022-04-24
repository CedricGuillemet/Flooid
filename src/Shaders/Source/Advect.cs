# include "bgfx_compute.sh"

uniform vec4 advection; // timestep, dissipation

SAMPLER2D(s_texVelocity, 0);
SAMPLER2D(s_texAdvect, 1);

IMAGE2D_WR(s_advectedOut, rgba16f, 2);

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    vec2 uvAdvected = coord /256. - texelFetch(s_texVelocity, coord, 0).xy/256. + 0.5/256.;
    vec4 value = texture2DLod(s_texAdvect, uvAdvected, 0);// * advection.y;
    imageStore(s_advectedOut, coord, value);
}
