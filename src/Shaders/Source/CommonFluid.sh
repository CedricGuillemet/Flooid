
IMAGE3D_RO(s_texTile1, rgba32f, 0);
IMAGE3D_RO(s_texTile2, rgba32f, 1);
IMAGE3D_RO(s_texWorldToTile, rgba32f,  2);

#define FetchInTile_(tile, _coord) \
{\
    vec4 worldTile = imageLoad(s_texWorldToTile, (_coord.xyz + ivec3(16, 16, 16)) / 16 - ivec3(1, 1, 1));\
    ivec3 localCoord = ivec3(_coord.x & 0xF, _coord.y & 0xF, _coord.z & 0xF);\
    ivec3 tileCoord = ivec3(worldTile.xyz * 255.);\
    return imageLoad(tile, tileCoord * 16 + localCoord);\
}

vec4 FetchInTile1(ivec3 coord) FetchInTile_(s_texTile1, coord)
vec4 FetchInTile2(ivec3 coord) FetchInTile_(s_texTile2, coord)

#define DX ivec3(1, 0, 0)
#define DY ivec3(0, 1, 0)
#define DZ ivec3(0, 0, 1)

