# include "bgfx_compute.sh"

SAMPLER2D(s_texIn, 0);
IMAGE2D_WR(s_texOut, rgba16f, 1);

NUM_THREADS(16, 16, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

    ivec2 dx = ivec2(1, 0);
    ivec2 dy = ivec2(0, 1);
    
    const float c0 = 0.25;
    const float c1 = 0.5;

    vec4 TL = texelFetch(s_texIn, coord - dx - dy, 0);
    vec4 TM = texelFetch(s_texIn, coord      - dy, 0);
    vec4 TR = texelFetch(s_texIn, coord - dy - dy, 0);

    vec4 ML = texelFetch(s_texIn, coord - dx, 0);
    vec4 MM = texelFetch(s_texIn, coord     , 0);
    vec4 MR = texelFetch(s_texIn, coord - dy, 0);

    vec4 BL = texelFetch(s_texIn, coord - dx + dy, 0);
    vec4 BM = texelFetch(s_texIn, coord      + dy, 0);
    vec4 BR = texelFetch(s_texIn, coord - dy + dy, 0);
    
    vec4 v = TL * c0 * c0 +
        TM * c0 * c1 +
        TR * c0 * c0 +
        
        ML * c1 * c0 +
        MM * c1 * c1 +
        MR * c1 * c0 +
        
        BL * c0 * c0 +
        BM * c0 * c1 +
        BR * c0 * c0;
    

    imageStore(s_texOut, coord, v);
}
