# include "bgfx_compute.sh"

SAMPLER2D(s_texU, 0);
SAMPLER2D(s_texRHS, 1);
IMAGE2D_WR(s_texOut, rgba32f, 2);

uniform vec4 invhsq;
NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

    ivec2 dx = ivec2(1, 0);
    ivec2 dy = ivec2(0, 1);

    vec4 res = texelFetch(s_texRHS, coord, 0) - (
        4. * texelFetch(s_texU, coord, 0)
        - texelFetch(s_texU, coord - dx, 0)
        - texelFetch(s_texU, coord + dx, 0)
        - texelFetch(s_texU, coord - dy, 0)
        - texelFetch(s_texU, coord + dy, 0)
        ) * invhsq.x;

    imageStore(s_texOut, coord, res);
}
