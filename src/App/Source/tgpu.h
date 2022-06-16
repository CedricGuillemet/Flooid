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
    bgfx::TextureHandle GetDisplayPages() const;
    bgfx::TextureHandle GetDisplayPageIndirection() const;
    bgfx::TextureHandle GetTags() const;
    int GetLevel() const;
    
    bgfx::UniformHandle m_jacobiParametersUniform;
    
    bgfx::UniformHandle m_invhsqUniform;
    bgfx::UniformHandle m_positionUniform;
    bgfx::UniformHandle m_directionUniform;
    //
    
    void TestPages(TextureProvider& textureProvider);
    
    
    bgfx::TextureHandle mDensityPages;
    bgfx::TextureHandle mVelocityPages;
    bgfx::TextureHandle mDensityAdvectedPages;
    bgfx::TextureHandle mVelocityAdvectedPages;

    // vcycle
    
    static const int MaxLevel = 5;
    bgfx::TextureHandle mWorldToPages[MaxLevel];
    bgfx::TextureHandle mDivergencePages;
    bgfx::TextureHandle mTempPages;
    bgfx::TextureHandle mJacobiPages[MaxLevel];
    bgfx::TextureHandle mGradientPages;
    bgfx::TextureHandle mResidualPages[MaxLevel];
    bgfx::TextureHandle mWorldToPageTags[MaxLevel];

    bgfx::DynamicIndexBufferHandle mBufferCounter[MaxLevel], mBufferPages[MaxLevel], mBufferAddressPages[MaxLevel];


    bgfx::DynamicIndexBufferHandle mBufferActivePages, mBufferFreedPages, mBufferActivePageAddresses;
    
    bgfx::ProgramHandle mClearCSProgram;
    bgfx::ProgramHandle mClearPagesCSProgram;
    bgfx::ProgramHandle mAllocatePagesCSProgram;
    bgfx::ProgramHandle mInitPagesCSProgram;
    bgfx::ProgramHandle mDensityGenPagedCSProgram;
    bgfx::ProgramHandle mVelocityGenPagedCSProgram;
    bgfx::ProgramHandle mJacobiPagedCSProgram;
    bgfx::ProgramHandle mGradientPagedCSProgram;
    bgfx::ProgramHandle mAdvectPagedCSProgram;
    bgfx::ProgramHandle mFreePagesCSProgram;
    bgfx::ProgramHandle mDispatchIndirectCSProgram;
    bgfx::ProgramHandle mCommitFreePagesCSProgram;
    bgfx::ProgramHandle mFrameInitCSProgram;
    bgfx::ProgramHandle mResidualPagedCSProgram;
    bgfx::ProgramHandle mAllocateSubPagesCSProgram;
    bgfx::ProgramHandle mDownscalePagedCSProgram;
    bgfx::ProgramHandle mUpscalePagedCSProgram;
    bgfx::ProgramHandle mDilatePagesCSProgram;
    bgfx::ProgramHandle mDivergencePagedCSProgram;
    
    bgfx::UniformHandle mGroupMinUniform;
    bgfx::UniformHandle mDebugDisplayUniform;
    bgfx::UniformHandle mTexWorldToPageUniform;
    bgfx::UniformHandle mInitPageCountUniform;


    bgfx::IndirectBufferHandle mDispatchIndirect[MaxLevel];
    
    int mDebugDisplay{4};
    bool mDebugGrid{false};
    bool mDebugPageAllocation{true};
    int mDebugLevel = 3;

    int mCurrentFrame{};
    int mDesiredFrame{1};


    void ComputeResidual(TextureProvider& textureProvider, bgfx::TextureHandle texU, bgfx::TextureHandle texRHS, bgfx::TextureHandle texWorldToPage, bgfx::TextureHandle texResidual,
        bgfx::DynamicIndexBufferHandle bufferPages, bgfx::DynamicIndexBufferHandle bufferAddressPages, bgfx::IndirectBufferHandle dispatchIndirect, float hsq);

    void Jacobi(TextureProvider& textureProvider, bgfx::TextureHandle texU, bgfx::TextureHandle texRHS, bgfx::TextureHandle texWorldToPage,
        bgfx::DynamicIndexBufferHandle bufferPages, bgfx::DynamicIndexBufferHandle bufferAddressPages, bgfx::IndirectBufferHandle dispatchIndirect, float hsq, int iterationCount);
    
    
    void ClearTexture(TextureProvider& textureProvider, bgfx::TextureHandle texture);
    void ClearPages(TextureProvider& textureProvider, bgfx::TextureHandle pages, bgfx::DynamicIndexBufferHandle bufferPages, bgfx::IndirectBufferHandle dispatchIndirect);
    
    void VCycle(TextureProvider& textureProvider, bgfx::TextureHandle rhs, int level, int maxLevel);
};
