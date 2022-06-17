ivec3 TileAddress(uint tileAddress)
{
    return ivec3(tileAddress & 1023, (tileAddress >> 10) & 1023, 0);
}

ivec3 WorldCoordFromTile(uint tileAddress, ivec3 localCoord)
{
    ivec3 tilePos = TileAddress(tileAddress);

    return tilePos * 16 + localCoord;
}

void WorldPosFromTile(uint tile, uint tileAddress, ivec2 coord, out ivec3 outBase, out vec3 voxelWorldPos)
{
    ivec3 tilePos = TileAddress(tileAddress);

    vec3 tileWorldPos = vec3(tilePos);

    // start of tile
    outBase = ivec3(tile&0xF, tile>>4, 0) * 16;

    // local offset in tile
    vec3 localOffset = vec3(coord.x & 0xF, coord.y & 0xF, 0.);
    outBase += localOffset;

    voxelWorldPos = tileWorldPos * (1./ 16.) + localOffset * (1./ 256.);
}