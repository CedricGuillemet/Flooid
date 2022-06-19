# include "bgfx_compute.sh"
# include "Paging.sh"

IMAGE3D_WR(s_texOut, rgba32f, 0);
BUFFER_RO(bufferTiles, uint, 1);

NUM_THREADS(8, 8, 8)
void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
    uint tile = bufferTiles[gl_WorkGroupID.y>>1];

    ivec3 destOut = GetOutAddr(tile, coord);

    imageStore(s_texOut, destOut, vec4(0., 0., 0., 0.));
}
