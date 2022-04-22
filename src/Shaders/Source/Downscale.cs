# include "bgfx_compute.sh"

SAMPLER2D(s_texU, 0);
IMAGE2D_WR(s_texOut, rgba16f, 1);

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

    ivec2 dx = ivec2(1, 0);
    ivec2 dy = ivec2(0, 1);

    vec4 uc = 0.5 * texelFetch(s_texU, coord, 0) +
        0.25 * (
          texelFetch(s_texU, coord - dx, 0)
        + texelFetch(s_texU, coord + dx, 0)
        + texelFetch(s_texU, coord - dy, 0)
        + texelFetch(s_texU, coord + dy, 0)) +
        0.125 * (
          texelFetch(s_texU, coord - dx - dy, 0)
        + texelFetch(s_texU, coord + dx - dy, 0)
        + texelFetch(s_texU, coord - dx + dy, 0)
        + texelFetch(s_texU, coord + dx + dy, 0));

    imageStore(s_texOut, coord, uc);
}
