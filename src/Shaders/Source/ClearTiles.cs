# include "bgfx_compute.sh"
# include "Paging.sh"

IMAGE2D_WR(s_texOut, rgba32f, 0);
BUFFER_RO(bufferTiles, uint, 1);

NUM_THREADS(16, 16, 1)
void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
    uint tile = bufferTiles[gl_WorkGroupID.y];

    ivec3 destOut = GetOutAddr(tile, coord);

    imageStore(s_texOut, destOut, vec4(0., 0., 0., 0.));
}
