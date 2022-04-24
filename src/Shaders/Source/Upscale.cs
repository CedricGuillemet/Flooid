# include "bgfx_compute.sh"

SAMPLER2D(s_texU, 0);
IMAGE2D_RW(s_texOut, rgba16f, 1);

uniform vec4 fineTexSize;
NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

    vec2 uv = gl_GlobalInvocationID.xy / fineTexSize.x;

    vec4 v = texture2DLod(s_texU, uv + 1.0 / fineTexSize.x, 0);

    vec4 color = imageLoad(s_texOut, coord);
    color += v;
    imageStore(s_texOut, coord, color);
}
