
IMAGE2D_RO(s_texPage1, rgba32f, 0);
IMAGE2D_RO(s_texPage2, rgba32f, 1);
IMAGE2D_RO(s_texWorldToPage, rgba32f,  2);

#define FetchInPage_(page, coord) \
{\
    vec4 worldPage = imageLoad(s_texWorldToPage, coord.xy / 16);\
    ivec2 localCoord = ivec2(coord.x & 0xF, coord.y & 0xF);\
    ivec2 pageCoord = ivec2(worldPage.xy * 255.);\
    return imageLoad(page, pageCoord * 16 + localCoord);\
}

vec4 FetchInPage1(ivec3 coord) FetchInPage_(s_texPage1, coord)
vec4 FetchInPage2(ivec3 coord) FetchInPage_(s_texPage2, coord)

const ivec3 dx = ivec3(1, 0, 0);
const ivec3 dy = ivec3(0, 1, 0);

