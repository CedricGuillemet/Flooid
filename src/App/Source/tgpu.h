#pragma once
#include <bgfx/bgfx.h>
#include <vector>
#include <assert.h>
#include "TextureProvider.h"
#include "Shaders.h"

class TGPU
{
public:
    TGPU();

    void Init(TextureProvider& textureProvider);
    void Tick(TextureProvider& textureProvider);

    void UI();
    bgfx::TextureHandle GetDisplayTiles() const;
    bgfx::TextureHandle GetDisplayTileIndirection() const;
    bgfx::TextureHandle GetDensityTiles() const;
    bgfx::TextureHandle GetTags() const;
    int GetLevel() const;
    
    bgfx::UniformHandle m_jacobiParametersUniform;
    
    bgfx::UniformHandle m_invhsqUniform;
    bgfx::UniformHandle m_positionUniform;
    bgfx::UniformHandle m_directionUniform;
    //
    
    void TestTiles(TextureProvider& textureProvider);
    
    
    bgfx::TextureHandle mDensityTiles;
    bgfx::TextureHandle mVelocityTiles;
    bgfx::TextureHandle mDensityAdvectedTiles;
    bgfx::TextureHandle mVelocityAdvectedTiles;

    // vcycle
    
    static const int MaxLevel = 4;
    bgfx::TextureHandle mWorldToTiles[MaxLevel];
    bgfx::TextureHandle mDivergenceTiles;
    bgfx::TextureHandle mTempTiles;
    bgfx::TextureHandle mJacobiTiles[MaxLevel];
    bgfx::TextureHandle mGradientTiles;
    bgfx::TextureHandle mResidualTiles[MaxLevel];
    bgfx::TextureHandle mWorldToTileTags[MaxLevel];

    bgfx::DynamicIndexBufferHandle mBufferCounter[MaxLevel], mBufferTiles[MaxLevel], mBufferAddressTiles[MaxLevel];


    bgfx::DynamicIndexBufferHandle mBufferActiveTiles, mBufferFreedTiles, mBufferActiveTileAddresses;
    
    bgfx::ProgramHandle mClearCSProgram;
    bgfx::ProgramHandle mClearTilesCSProgram;
    bgfx::ProgramHandle mAllocateTilesCSProgram;
    bgfx::ProgramHandle mInitTilesCSProgram;
    bgfx::ProgramHandle mDensityGenTileCSProgram;
    bgfx::ProgramHandle mVelocityGenTileCSProgram;
    bgfx::ProgramHandle mJacobiTileCSProgram;
    bgfx::ProgramHandle mGradientTileCSProgram;
    bgfx::ProgramHandle mAdvectTileCSProgram;
    bgfx::ProgramHandle mFreeTilesCSProgram;
    bgfx::ProgramHandle mDispatchIndirectCSProgram;
    bgfx::ProgramHandle mCommitFreeTilesCSProgram;
    bgfx::ProgramHandle mResidualTileCSProgram;
    bgfx::ProgramHandle mAllocateCoarserTilesCSProgram;
    bgfx::ProgramHandle mDownscaleTileCSProgram;
    bgfx::ProgramHandle mUpscaleTileCSProgram;
    bgfx::ProgramHandle mDilateTilesCSProgram;
    bgfx::ProgramHandle mDivergenceTileCSProgram;
    bgfx::ProgramHandle mBuoyancyCSProgram;
    bgfx::ProgramHandle mCopyTileCSProgram;
    
    bgfx::UniformHandle mGroupMinUniform;
    bgfx::UniformHandle mDebugDisplayUniform;
    bgfx::UniformHandle mTexWorldToTileUniform;
    bgfx::UniformHandle mInitTileCountUniform;
    bgfx::UniformHandle mAdvectionFactorUniform;
    
    bgfx::IndirectBufferHandle mDispatchIndirect[MaxLevel];
    
    int mDebugDisplay{0};
    bool mDebugGrid{false};
    bool mDebugTileAllocation{true};
    int mDebugLevel = 0;

    int mCurrentFrame{};
    int mDesiredFrame{1};


    void ComputeResidual(TextureProvider& textureProvider, bgfx::TextureHandle texU, bgfx::TextureHandle texRHS, bgfx::TextureHandle texWorldToTile, bgfx::TextureHandle texResidual,
        bgfx::DynamicIndexBufferHandle bufferTiles, bgfx::DynamicIndexBufferHandle bufferAddressTiles, bgfx::IndirectBufferHandle dispatchIndirect, float hsq);

    void Jacobi(TextureProvider& textureProvider, bgfx::TextureHandle texU, bgfx::TextureHandle texRHS, bgfx::TextureHandle texWorldToTile,
        bgfx::DynamicIndexBufferHandle bufferTiles, bgfx::DynamicIndexBufferHandle bufferAddressTiles, bgfx::IndirectBufferHandle dispatchIndirect, float hsq, int iterationCount);
    
    
    void ClearTexture(TextureProvider& textureProvider, bgfx::TextureHandle texture);
    void ClearTiles(TextureProvider& textureProvider, bgfx::TextureHandle tiles, bgfx::DynamicIndexBufferHandle bufferTiles, bgfx::IndirectBufferHandle dispatchIndirect);
    
    void VCycle(TextureProvider& textureProvider, bgfx::TextureHandle rhs, int level, int maxLevel);
};
