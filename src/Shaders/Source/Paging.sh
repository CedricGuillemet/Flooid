void WorldPosFromPage(uint page, uint pageAddress, ivec2 coord, out ivec3 outBase, out vec3 voxelWorldPos)
{
    ivec3 pagePos = ivec3(pageAddress & 1023, (pageAddress >> 10) & 1023, 0);

    vec3 pageWorldPos = vec3(pagePos);

    // start of page
    outBase = ivec3(page&0xF, page>>4, 0) * 16;

    // local offset in page
    vec3 localOffset = vec3(coord.x & 0xF, coord.y & 0xF, 0.);
    outBase += localOffset;

    voxelWorldPos = pageWorldPos * (1./ 16.) + localOffset * (1./ 256.);
}