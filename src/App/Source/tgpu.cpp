#include "tgpu.h"

static const int TEX_SIZE = 256;

Imm::vec3 densityCenter{ 0.5f, 0.1f, 0.5f };
Imm::vec3 densityExtend{ 0.05f, 0.05f, 0.05f };

TGPU::TGPU()
{

}

void TGPU::Init(TextureProvider& textureProvider)
{
    const int tileSize = 16;
    const int masterSize = 256;
    /*
    mDensityTiles = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mVelocityTiles = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mDivergenceTiles = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mGradientTiles = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    
    mTempTiles = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mDensityAdvectedTiles = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mVelocityAdvectedTiles = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
*/
    mDensityTiles = bgfx::createTexture3D(masterSize, masterSize, masterSize, false, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mVelocityTiles = bgfx::createTexture3D(masterSize, masterSize, masterSize, false, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mDivergenceTiles = bgfx::createTexture3D(masterSize, masterSize, masterSize, false, bgfx::TextureFormat::R32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mGradientTiles = bgfx::createTexture3D(masterSize, masterSize, masterSize, false, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    
    mTempTiles = bgfx::createTexture3D(masterSize, masterSize, masterSize, false, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mDensityAdvectedTiles = bgfx::createTexture3D(masterSize, masterSize, masterSize, false, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mVelocityAdvectedTiles = bgfx::createTexture3D(masterSize, masterSize, masterSize, false, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    
    mAllocateTilesCSProgram = App::LoadProgram("AllocateTiles_cs", nullptr);
    mInitTilesCSProgram = App::LoadProgram("InitTiles_cs", nullptr);
    mDensityGenTileCSProgram = App::LoadProgram("DensityGenTile_cs", nullptr);
    mVelocityGenTileCSProgram = App::LoadProgram("VelocityGenTile_cs", nullptr);
    mDilateTilesCSProgram = App::LoadProgram("DilateTiles_cs", nullptr);
    mDivergenceTileCSProgram = App::LoadProgram("DivergenceTile_cs", nullptr);
    mJacobiTileCSProgram = App::LoadProgram("JacobiTile_cs", nullptr);
    mGradientTileCSProgram = App::LoadProgram("GradientTile_cs", nullptr);
    mAdvectTileCSProgram = App::LoadProgram("AdvectTile_cs", nullptr);
    mFreeTilesCSProgram = App::LoadProgram("FreeTiles_cs", nullptr);
    mDispatchIndirectCSProgram = App::LoadProgram("DispatchIndirect_cs", nullptr);
    mCommitFreeTilesCSProgram = App::LoadProgram("CommitFreeTiles_cs", nullptr);
    mResidualTileCSProgram = App::LoadProgram("ResidualTile_cs", nullptr);
    mAllocateCoarserTilesCSProgram = App::LoadProgram("AllocateCoarserTiles_cs", nullptr);
    mDownscaleTileCSProgram = App::LoadProgram("DownscaleTile_cs", nullptr);
    mUpscaleTileCSProgram = App::LoadProgram("UpscaleTile_cs", nullptr);
    mClearCSProgram = App::LoadProgram("Clear_cs", nullptr);
    mClearTilesCSProgram = App::LoadProgram("ClearTiles_cs", nullptr);
    
    m_jacobiParametersUniform = bgfx::createUniform("jacobiParameters", bgfx::UniformType::Vec4);
    m_invhsqUniform = bgfx::createUniform("invhsq", bgfx::UniformType::Vec4);
    m_positionUniform = bgfx::createUniform("position", bgfx::UniformType::Vec4);
    m_directionUniform = bgfx::createUniform("direction", bgfx::UniformType::Vec4);
    mTexWorldToTileUniform = bgfx::createUniform("s_texWorldToTile", bgfx::UniformType::Sampler);
    mDebugDisplayUniform = bgfx::createUniform("debugDisplay", bgfx::UniformType::Vec4);

    uint32_t tileCount = (256/tileSize) * (256/tileSize) * (256/tileSize);
        
    mBufferActiveTiles = bgfx::createDynamicIndexBuffer(tileCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    mBufferFreedTiles = bgfx::createDynamicIndexBuffer(tileCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    mBufferActiveTileAddresses = bgfx::createDynamicIndexBuffer(tileCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    
    mGroupMinUniform = bgfx::createUniform("groupMin", bgfx::UniformType::Vec4);
    mInitTileCountUniform = bgfx::createUniform("initTileCount", bgfx::UniformType::Vec4);
    
    for (int i = 0; i < MaxLevel; i++)
    {
        mDispatchIndirect[i] = bgfx::createIndirectBuffer(2);
        
        mBufferCounter[i] = bgfx::createDynamicIndexBuffer(3, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
        mBufferTiles[i] = bgfx::createDynamicIndexBuffer(tileCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
        mBufferAddressTiles[i] = bgfx::createDynamicIndexBuffer(tileCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);

        int worldMapSize = (masterSize / tileSize) >> i;
        /*
        mWorldToTiles[i] = bgfx::createTexture2D(worldMapSize, worldMapSize, false, 0, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_COMPUTE_WRITE);
        mWorldToTileTags[i] = bgfx::createTexture2D(worldMapSize, worldMapSize, false, 0, bgfx::TextureFormat::R8, BGFX_TEXTURE_COMPUTE_WRITE);

        mJacobiTiles[i] = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
        mResidualTiles[i] = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
         */
        mWorldToTiles[i] = bgfx::createTexture3D(worldMapSize, worldMapSize, worldMapSize, false, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_COMPUTE_WRITE);
        mWorldToTileTags[i] = bgfx::createTexture3D(worldMapSize, worldMapSize, worldMapSize, false, bgfx::TextureFormat::R8, BGFX_TEXTURE_COMPUTE_WRITE);

        mJacobiTiles[i] = bgfx::createTexture3D(masterSize, masterSize, masterSize, false, bgfx::TextureFormat::R32F, BGFX_TEXTURE_COMPUTE_WRITE);
        mResidualTiles[i] = bgfx::createTexture3D(masterSize, masterSize, masterSize, false, bgfx::TextureFormat::R32F, BGFX_TEXTURE_COMPUTE_WRITE);
    }
}

void TGPU::ComputeResidual(TextureProvider& textureProvider, bgfx::TextureHandle texU, bgfx::TextureHandle texRHS, bgfx::TextureHandle texWorldToTile, bgfx::TextureHandle texResidual, 
    bgfx::DynamicIndexBufferHandle bufferTiles, bgfx::DynamicIndexBufferHandle bufferAddressTiles, bgfx::IndirectBufferHandle dispatchIndirect, float hsq)
{
    float invhsq[4] = { 1.f / hsq, 0.f, 0.f, 0.f };
    bgfx::setUniform(m_invhsqUniform, invhsq);

    bgfx::setImage(0, texU, 0, bgfx::Access::Read);
    bgfx::setImage(1, texRHS, 0, bgfx::Access::Read);
    bgfx::setImage(2, texWorldToTile, 0, bgfx::Access::Read);
    bgfx::setBuffer(3, bufferTiles, bgfx::Access::Read);
    bgfx::setBuffer(4, bufferAddressTiles, bgfx::Access::Read);
    bgfx::setImage(5, texResidual, 0, bgfx::Access::Write);

    bgfx::dispatch(textureProvider.GetViewId(), mResidualTileCSProgram, dispatchIndirect);
}

void TGPU::Jacobi(TextureProvider& textureProvider, bgfx::TextureHandle texU, bgfx::TextureHandle texRHS, bgfx::TextureHandle texWorldToTile,
    bgfx::DynamicIndexBufferHandle bufferTiles, bgfx::DynamicIndexBufferHandle bufferAddressTiles, bgfx::IndirectBufferHandle dispatchIndirect, float hsq, int iterationCount)
{
    float jacobiParameters[4] = { hsq, 0.f, 0.f, 0.f };
    bgfx::setUniform(m_jacobiParametersUniform, jacobiParameters);

    bgfx::TextureHandle jacobis[2] = { texU, mTempTiles };

    for (int i = 0; i < iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;

        bgfx::setImage(0, jacobis[indexSource], 0, bgfx::Access::Read);
        bgfx::setImage(1, texRHS, 0, bgfx::Access::Read);
        bgfx::setImage(2, texWorldToTile, 0, bgfx::Access::Read);
        bgfx::setBuffer(3, bufferAddressTiles, bgfx::Access::Read);
        bgfx::setBuffer(4, bufferTiles, bgfx::Access::Read);
        bgfx::setImage(5, jacobis[indexDestination], 0, bgfx::Access::Write);
        bgfx::dispatch(textureProvider.GetViewId(), mJacobiTileCSProgram, dispatchIndirect);
    }
}

void TGPU::ClearTexture(TextureProvider& textureProvider, bgfx::TextureHandle texture)
{
    bgfx::setImage(0, texture, 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), mClearCSProgram, 256 / 8, 256 / 8, 256 / 8);
}

void TGPU::ClearTiles(TextureProvider& textureProvider, bgfx::TextureHandle tiles, bgfx::DynamicIndexBufferHandle bufferTiles, bgfx::IndirectBufferHandle dispatchIndirect)
{
    bgfx::setImage(0, tiles, 0, bgfx::Access::Write);
    bgfx::setBuffer(1, bufferTiles, bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mClearTilesCSProgram, dispatchIndirect);
}

void TGPU::TestTiles(TextureProvider& textureProvider)
{
    // t allocate

    Imm::vec3 wmin = densityCenter;
    wmin -= densityExtend;//{0.1f, 0.1f, 0.0f};
    Imm::vec3 wmax = densityCenter;
    wmax += densityExtend;//{0.4f, 0.4f, 0.0f};*/

    /*Imm::vec3 wmin = { 0.0f, 0.0f, 0.0f };
    Imm::vec3 wmax = { 1.f, 1.f, 0.0f };
    */
    int groupMinx = int(wmin.x * 256.f);
    groupMinx -= groupMinx % 16;

    int groupMiny = int(wmin.y * 256.f);
    groupMiny -= groupMiny % 16;

    int groupMinz = int(wmin.z * 256.f);
    groupMinz -= groupMinz % 16;

    int groupMaxx = int(wmax.x * 256.f);
    int modmx = groupMaxx % 16;
    groupMaxx -= modmx;
    groupMaxx += modmx ? 16 : 0;

    int groupMaxy = int(wmax.y * 256.f);
    int modmy = groupMaxy % 16;
    groupMaxy -= modmy;
    groupMaxy += modmy ? 16 : 0;

    int groupMaxz = int(wmax.z * 256.f);
    int modmz = groupMaxz % 16;
    groupMaxz -= modmz;
    groupMaxz += modmz ? 16 : 0;

    groupMinx /= 16;
    groupMiny /= 16;
    groupMinz /= 16;
    groupMaxx /= 16;
    groupMaxy /= 16;
    groupMaxz /= 16;

    //float invhsq[4] = { 1.f / hsq, 0.f, 0.f, 0.f };
    //bgfx::setUniform(m_invhsqUniform, invhsq);
    int invocationx = groupMaxx - groupMinx;
    int invocationy = groupMaxy - groupMiny;
    int invocationz = groupMaxz - groupMinz;

    //

    static bool initialized = false;
    // init tiles
    if (!initialized)
    {
        for (int i = 0; i < MaxLevel; i++)
        {
            ClearTexture(textureProvider, mWorldToTiles[i]);
            ClearTexture(textureProvider, mWorldToTileTags[i]);
        }
        
        
        initialized = true;
        
        float tileCount[4] = { (16 * 16 * 16) - 1, 0.f, 0.f, 0.f };
        bgfx::setUniform(mInitTileCountUniform, tileCount);
        bgfx::setBuffer(0, mBufferTiles[0], bgfx::Access::Write);
        bgfx::setBuffer(1, mBufferCounter[0], bgfx::Access::ReadWrite);
        bgfx::dispatch(textureProvider.GetViewId(), mInitTilesCSProgram, 1, 1, 1);
        
        // allocate tiles
        float groupMin[4] = { float(groupMinx), float(groupMiny), float(groupMinz), 0.f };
        bgfx::setUniform(mGroupMinUniform, groupMin);

        //bgfx::setBuffer(0, mFreeTiles, bgfx::Access::ReadWrite);
        bgfx::setBuffer(0, mBufferAddressTiles[0], bgfx::Access::Write);
        bgfx::setImage(1, mWorldToTiles[0], 0, bgfx::Access::Write);
        bgfx::setBuffer(2, mBufferTiles[0], bgfx::Access::Write);
        bgfx::setBuffer(3, mBufferCounter[0], bgfx::Access::ReadWrite);
        bgfx::setImage(4, mWorldToTileTags[0], 0, bgfx::Access::Write);
        bgfx::dispatch(textureProvider.GetViewId(), mAllocateTilesCSProgram, invocationx, invocationy, invocationz);
        
        // clear density and velocity
        ClearTexture(textureProvider, mDensityTiles);
        ClearTexture(textureProvider, mVelocityTiles);
    }
    // dispatch indirect
    bgfx::setBuffer(0, mDispatchIndirect[0], bgfx::Access::ReadWrite);
    bgfx::setBuffer(1, mBufferCounter[0], bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mDispatchIndirectCSProgram, 1, 1, 1);
    
    // density
    float position[4] = { densityCenter.x, densityCenter.y, densityCenter.z, densityExtend.x };
    bgfx::setUniform(m_positionUniform, position);

    bgfx::setImage(0, mDensityTiles, 0, bgfx::Access::Write);
    bgfx::setBuffer(1, mBufferAddressTiles[0], bgfx::Access::Read);
    bgfx::setBuffer(2, mBufferTiles[0], bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mDensityGenTileCSProgram, mDispatchIndirect[0]);

    // velocity
    float direction[4] = { 0.f, 1.f, 0.f, 0.f };
    bgfx::setUniform(m_directionUniform, direction);

    bgfx::setImage(0, mVelocityTiles, 0, bgfx::Access::Write);
    bgfx::setBuffer(1, mBufferAddressTiles[0], bgfx::Access::Read);
    bgfx::setBuffer(2, mBufferTiles[0], bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mVelocityGenTileCSProgram, mDispatchIndirect[0]);
    
    // advect density
    bgfx::setImage(0, mDensityTiles, 0, bgfx::Access::Read);
    bgfx::setImage(1, mVelocityTiles, 0, bgfx::Access::Read);
    bgfx::setImage(2, mWorldToTiles[0], 0, bgfx::Access::Read);
    bgfx::setBuffer(3, mBufferAddressTiles[0], bgfx::Access::Read);
    bgfx::setImage(4, mDensityAdvectedTiles, 0, bgfx::Access::Write);
    bgfx::setBuffer(5, mBufferTiles[0], bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mAdvectTileCSProgram, mDispatchIndirect[0]);
    
    // advect velocity
    bgfx::setImage(0, mVelocityTiles, 0, bgfx::Access::Read);
    bgfx::setImage(1, mVelocityTiles, 0, bgfx::Access::Read);
    bgfx::setImage(2, mWorldToTiles[0], 0, bgfx::Access::Read);
    bgfx::setBuffer(3, mBufferAddressTiles[0], bgfx::Access::Read);
    bgfx::setImage(4, mVelocityAdvectedTiles, 0, bgfx::Access::Write);
    bgfx::setBuffer(5, mBufferTiles[0], bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mAdvectTileCSProgram, mDispatchIndirect[0]);

    // free tiles
    bgfx::setBuffer(0, mBufferAddressTiles[0], bgfx::Access::Read);
    bgfx::setBuffer(1, mBufferTiles[0], bgfx::Access::Read);
    bgfx::setImage(2, mWorldToTileTags[0], 0, bgfx::Access::Write);
    bgfx::setImage(3, mDensityAdvectedTiles, 0, bgfx::Access::Read);
    bgfx::setBuffer(4, mBufferCounter[0], bgfx::Access::ReadWrite);
    bgfx::setBuffer(5, mBufferActiveTiles, bgfx::Access::ReadWrite);
    bgfx::setBuffer(6, mBufferFreedTiles, bgfx::Access::ReadWrite);
    bgfx::setBuffer(7, mBufferActiveTileAddresses, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mFreeTilesCSProgram, mDispatchIndirect[0], 1);
    
    // commit tiles
    bgfx::setBuffer(0, mBufferTiles[0], bgfx::Access::ReadWrite);
    bgfx::setBuffer(1, mBufferCounter[0], bgfx::Access::ReadWrite);
    bgfx::setBuffer(2, mBufferAddressTiles[0], bgfx::Access::ReadWrite);
    bgfx::setBuffer(3, mBufferActiveTiles, bgfx::Access::ReadWrite);
    bgfx::setBuffer(4, mBufferFreedTiles, bgfx::Access::ReadWrite);
    bgfx::setBuffer(5, mBufferActiveTileAddresses, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mCommitFreeTilesCSProgram, 1, 1);
    
    // Dilate tiles
    bgfx::setBuffer(0, mBufferAddressTiles[0], bgfx::Access::Write);
    bgfx::setImage(1, mWorldToTiles[0], 0, bgfx::Access::Write);
    bgfx::setBuffer(2, mBufferTiles[0], bgfx::Access::Write);
    bgfx::setBuffer(3, mBufferCounter[0], bgfx::Access::ReadWrite);
    bgfx::setImage(4, mWorldToTileTags[0], 0, bgfx::Access::Write);
    bgfx::setImage(5, mVelocityAdvectedTiles, 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), mDilateTilesCSProgram, (256 / 16) / 8, (256 / 16) / 8, (256 / 16) / 8);
    
    // dispatch indirect
    bgfx::setBuffer(0, mDispatchIndirect[0], bgfx::Access::ReadWrite);
    bgfx::setBuffer(1, mBufferCounter[0], bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mDispatchIndirectCSProgram, 1, 1);
    
    // Divergence
    bgfx::setImage(0, mVelocityAdvectedTiles, 0, bgfx::Access::Read);
    bgfx::setImage(2, mWorldToTiles[0], 0, bgfx::Access::Read);
    bgfx::setBuffer(3, mBufferAddressTiles[0], bgfx::Access::Read);
    bgfx::setImage(4, mDivergenceTiles, 0, bgfx::Access::Write);
    bgfx::setBuffer(5, mBufferTiles[0], bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mDivergenceTileCSProgram, mDispatchIndirect[0]);
    
    // VCycle
    VCycle(textureProvider, mDivergenceTiles, 0, MaxLevel - 1);
 
    // -------------------------------------------
    // 
    // gradient
    bgfx::setImage(0, mJacobiTiles[0], 0, bgfx::Access::Read);
    bgfx::setImage(1, mVelocityAdvectedTiles, 0, bgfx::Access::Read);
    bgfx::setImage(2, mWorldToTiles[0], 0, bgfx::Access::Read);
    bgfx::setBuffer(3, mBufferAddressTiles[0], bgfx::Access::Read);
    bgfx::setImage(4, mGradientTiles, 0, bgfx::Access::Write);
    bgfx::setBuffer(5, mBufferTiles[0], bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mGradientTileCSProgram, mDispatchIndirect[0]);

    std::swap(mGradientTiles, mVelocityTiles);
    std::swap(mDensityTiles, mDensityAdvectedTiles);
}

void TGPU::VCycle(TextureProvider& textureProvider, bgfx::TextureHandle rhs, int level, int maxLevel)
{
    const float hsq = float(level + 1);
    int ssteps = 4;
    int steps = 60;
    
    if (level == maxLevel)
    {
        ClearTiles(textureProvider, mJacobiTiles[level], mBufferTiles[level], mDispatchIndirect[level]);
        Jacobi(textureProvider, mJacobiTiles[level], rhs/*mResidualTiles[level]*/, mWorldToTiles[level], mBufferTiles[level], mBufferAddressTiles[level], mDispatchIndirect[level], hsq, steps);

        return;
    }

    //bgfx::TextureHandle rhs = mDivergenceTiles;

    ClearTiles(textureProvider, mJacobiTiles[level], mBufferTiles[level], mDispatchIndirect[level]);
    Jacobi(textureProvider, mJacobiTiles[level], rhs, mWorldToTiles[level], mBufferTiles[level], mBufferAddressTiles[level], mDispatchIndirect[level], hsq, ssteps);

    // Residual
    ComputeResidual(textureProvider, mJacobiTiles[level], rhs, mWorldToTiles[level], mResidualTiles[level], mBufferTiles[level], mBufferAddressTiles[level], mDispatchIndirect[level], hsq);

    // next level -------------------------------

    // init
    float tileCount[4] = { 63.f, 0.f, 0.f, 0.f };
    bgfx::setUniform(mInitTileCountUniform, tileCount);
    bgfx::setBuffer(0, mBufferTiles[level + 1], bgfx::Access::Write);
    bgfx::setBuffer(1, mBufferCounter[level + 1], bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mInitTilesCSProgram, 1, 1);

    // new indirection map
    bgfx::setBuffer(0, mBufferAddressTiles[level + 1], bgfx::Access::Write);
    bgfx::setImage(1, mWorldToTiles[level + 1], 0, bgfx::Access::Write);
    bgfx::setBuffer(2, mBufferTiles[level + 1], bgfx::Access::ReadWrite);
    bgfx::setBuffer(3, mBufferCounter[level + 1], bgfx::Access::ReadWrite);
    bgfx::setImage(4, mWorldToTileTags[level], 0, bgfx::Access::Read);
    bgfx::setImage(5, mWorldToTileTags[level + 1], 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), mAllocateCoarserTilesCSProgram, 1, 1);

    // dispatch indirect
    bgfx::setBuffer(0, mDispatchIndirect[level + 1], bgfx::Access::ReadWrite);
    bgfx::setBuffer(1, mBufferCounter[level + 1], bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mDispatchIndirectCSProgram, 1, 1);

    auto rhsNext = textureProvider.Acquire(PlugType::Any, TEX_SIZE);
    // downscale filter
    bgfx::setImage(0, mResidualTiles[level], 0, bgfx::Access::Read);
    bgfx::setImage(2, mWorldToTiles[level], 0, bgfx::Access::Read);
    bgfx::setImage(3, /*mResidualTiles[level + 1]*/rhsNext->GetTexture(), 0, bgfx::Access::Write);
    bgfx::setBuffer(4, mBufferAddressTiles[level + 1], bgfx::Access::Read);
    bgfx::setBuffer(5, mBufferTiles[level + 1], bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mDownscaleTileCSProgram, mDispatchIndirect[level + 1]);
    
    VCycle(textureProvider, rhsNext->GetTexture(), level + 1, maxLevel);
    
    textureProvider.Release(rhsNext);
    // upscale level1 -> level0
    bgfx::setImage(0, mJacobiTiles[level + 1], 0, bgfx::Access::Read); //unext
    bgfx::setImage(2, mWorldToTiles[level + 1], 0, bgfx::Access::Read);
    bgfx::setBuffer(3,mBufferAddressTiles[level], bgfx::Access::Read);
    bgfx::setBuffer(4,mBufferTiles[level], bgfx::Access::Read);
    bgfx::setImage(5, mJacobiTiles[level], 0, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mUpscaleTileCSProgram, mDispatchIndirect[level]);
    
    Jacobi(textureProvider, mJacobiTiles[level], rhs, mWorldToTiles[level], mBufferTiles[level], mBufferAddressTiles[level], mDispatchIndirect[level], hsq, ssteps);
}

void TGPU::Tick(TextureProvider& textureProvider)
{
    // debug display
    float debugDisplay[4] = { mDebugGrid ? 1.f : 0.f, mDebugTileAllocation ? 1.f : 0.f, float(mDebugDisplay), float(GetLevel()) };
    bgfx::setUniform(mDebugDisplayUniform, debugDisplay);
    bgfx::touch(textureProvider.GetViewId());

    if (mCurrentFrame < mDesiredFrame)
    {
        mCurrentFrame++;
        TestTiles(textureProvider);
    }
}


void TGPU::UI()
{
    if (ImGui::Button("Next"))
    {
        mDesiredFrame++;
    }
    ImGui::SameLine();
    if (ImGui::Button("Play"))
    {
        mDesiredFrame+=9999999;
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop"))
    {
        mCurrentFrame = mDesiredFrame;
    }

    if (ImGui::RadioButton("Density", mDebugDisplay == 0)) mDebugDisplay = 0;
    if (ImGui::RadioButton("Velocity", mDebugDisplay == 1)) mDebugDisplay = 1;
    
    if (ImGui::RadioButton("Divergence", mDebugDisplay == 3)) mDebugDisplay = 3;
    const char* const levels[] = {"Level 0", "Level 1", "Level 2", "Level 3", "Level 4", "Level 5", "Level 6", "Level 7"};
    ImGui::Combo("Level", &mDebugLevel, levels, MaxLevel);
    if (ImGui::RadioButton("Tile Tag", mDebugDisplay == 2)) mDebugDisplay = 2;
    if (ImGui::RadioButton("Jacobi", mDebugDisplay == 4)) mDebugDisplay = 4;
    if (ImGui::RadioButton("Residual", mDebugDisplay == 5)) mDebugDisplay = 5;
    if (ImGui::RadioButton("Gradient", mDebugDisplay == 6)) mDebugDisplay = 6;

    ImGui::Checkbox("Grid", &mDebugGrid);
    ImGui::Checkbox("Tile Allocation", &mDebugTileAllocation);
}

bgfx::TextureHandle TGPU::GetDisplayTiles() const
{
    switch (mDebugDisplay)
    {
    case 0: return mDensityTiles;
    case 1: return mVelocityTiles;
    case 2: return mDensityTiles;
    case 3: return mDivergenceTiles;
    case 4: return mJacobiTiles[mDebugLevel];
    case 5: return mResidualTiles[mDebugLevel];
    case 6: return mGradientTiles;
    }
    return { bgfx::kInvalidHandle };
}

bgfx::TextureHandle TGPU::GetDensityTiles() const
{
    return mDensityTiles;
}

bgfx::TextureHandle TGPU::GetDisplayTileIndirection() const
{
    switch (mDebugDisplay)
    {
        case 4:
        case 5: return mWorldToTiles[mDebugLevel];
        default: return mWorldToTiles[0];
    }
    return { bgfx::kInvalidHandle };
}

bgfx::TextureHandle TGPU::GetTags() const
{
    switch (mDebugDisplay)
    {
        case 2:
        case 4:
        case 5: return mWorldToTileTags[mDebugLevel];
        default: return mWorldToTileTags[0];
    }
    return { bgfx::kInvalidHandle };
}

int TGPU::GetLevel() const
{
    if (mDebugDisplay == 4 || mDebugDisplay == 5 || mDebugDisplay == 2)
    {
        return mDebugLevel;
    }
    return 0;
}
