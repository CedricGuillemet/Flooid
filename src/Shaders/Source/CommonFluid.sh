
IMAGE2D_RO(s_texPage1, rgba32f, 0);
IMAGE2D_RO(s_texPage2, rgba32f, 1);
IMAGE2D_RO(s_texWorldToPage, rgba32f,  2);

#define FetchInPage_(page, _coord) \
{\
    vec4 worldPage = imageLoad(s_texWorldToPage, (_coord.xy + ivec2(16,16)) / 16 - ivec2(1,1));\
    ivec2 localCoord = ivec2(_coord.x & 0xF, _coord.y & 0xF);\
    ivec2 pageCoord = ivec2(worldPage.xy * 255.);\
    return imageLoad(page, pageCoord * 16 + localCoord);\
}

vec4 FetchInPage1(ivec3 coord) FetchInPage_(s_texPage1, coord)
vec4 FetchInPage2(ivec3 coord) FetchInPage_(s_texPage2, coord)

#define DX ivec3(1, 0, 0)
#define DY ivec3(0, 1, 0)

