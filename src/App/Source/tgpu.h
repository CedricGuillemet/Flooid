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

    bgfx::ProgramHandle m_downscaleCSProgram;
    bgfx::ProgramHandle m_upscaleCSProgram;
    bgfx::ProgramHandle m_residualCSProgram;

    bgfx::UniformHandle m_texUUniform;
    bgfx::UniformHandle m_texRHSUniform;
    bgfx::UniformHandle m_invhsqUniform;
    bgfx::UniformHandle m_fineTexSizeUniform;


    Texture* m_densityTexture{};
    Texture* m_velocityTexture{};

    Texture* m_dummy;

    bgfx::ProgramHandle m_divergenceCSProgram;
    bgfx::ProgramHandle m_gradientCSProgram;
    bgfx::ProgramHandle m_jacobiCSProgram;
    bgfx::ProgramHandle m_advectCSProgram;

    bgfx::ProgramHandle m_velocityGenCSProgram;
    bgfx::UniformHandle m_positionUniform;
    bgfx::UniformHandle m_directionUniform;

    bgfx::ProgramHandle m_densityGenCSProgram;
    bgfx::UniformHandle m_invWorldMatrixUniform;

    bgfx::UniformHandle m_advectionUniform;
    bgfx::UniformHandle m_texAdvectUniform;

    bgfx::UniformHandle m_jacobiParametersUniform;
    bgfx::UniformHandle m_texJacoviUniform;
    bgfx::UniformHandle m_texDivergenceUniform;
    bgfx::UniformHandle m_texColorUniform;
    bgfx::UniformHandle m_texPressureUniform;

    bgfx::ProgramHandle m_clearCSProgram;


    bgfx::UniformHandle m_texVelocityUniform;
    
    
    Texture* tempRHS;
    Texture* jacobi[2];
    Texture* tempGradient;
    
    //
    void DensityGen(TextureProvider& textureProvider);
    void VelocityGen(TextureProvider& textureProvider);
    
    void Advect(TextureProvider& textureProvider, Texture* source, Texture* velocity, Texture* output);
    void Gradient(TextureProvider& textureProvider, Texture* u, Texture* velocity, Texture* destination);
    void Divergence(TextureProvider& textureProvider, Texture* velocity, Texture* destination);
    void Jacobi(TextureProvider& textureProvider, Texture* jacobi[2], const Texture* rhs, int iterationCount);



    void coarsen(TextureProvider& textureProvider, const Texture* uf, Texture* uc);
    void refine_and_add(TextureProvider& textureProvider, const Texture* u, Texture* uf);
    void compute_residual(TextureProvider& textureProvider, const Texture* u, const Texture* rhs, Texture* res, float hsq);
    void compute_and_coarsen_residual(TextureProvider& textureProvider, const Texture* u, const Texture* rhs, Texture* resc, float hsq);
    void vcycle(TextureProvider& textureProvider, const Texture* rhs, Texture* u, int fineSize, int level, int max);
    void Jacobi(TextureProvider& textureProvider, Texture* u, const Texture* rhs, int iterationCount, float hsq);
    
    void TestVCycle(TextureProvider& textureProvider);
    void TestPages(TextureProvider& textureProvider);
    bgfx::TextureHandle mWorldToPages;
    bgfx::TextureHandle mWorldToPageTags;
    bgfx::TextureHandle mDensityPages;
    bgfx::TextureHandle mVelocityPages;
    bgfx::TextureHandle mDensityAdvectedPages;
    bgfx::TextureHandle mVelocityAdvectedPages;

    bgfx::TextureHandle mDivergencePages;
    bgfx::TextureHandle mJacobiPages[2];
    bgfx::TextureHandle mGradientPages;

    bgfx::DynamicIndexBufferHandle mBufferCounter, mFreePages, mBufferPages, mBufferAddressPages;
    
    bgfx::ProgramHandle mAllocatePagesCSProgram;
    bgfx::ProgramHandle mInitPagesCSProgram;
    bgfx::ProgramHandle mDensityGenPagedCSProgram;
    bgfx::ProgramHandle mVelocityGenPagedCSProgram;
    bgfx::ProgramHandle mJacobiPagedCSProgram;
    bgfx::ProgramHandle mGradientPagedCSProgram;
    bgfx::ProgramHandle mAdvectPagedCSProgram;
    bgfx::ProgramHandle mFreePagesCSProgram;

    bgfx::ProgramHandle mDilatePagesCSProgram;
    bgfx::ProgramHandle mDivergencePagedCSProgram;
    
    bgfx::UniformHandle mGroupMinUniform;
    bgfx::UniformHandle mTexOutUniform; // s_texOut
    
    bgfx::UniformHandle mDebugDisplayUniform;
    bgfx::UniformHandle mTexWorldToPageUniform;
    bgfx::UniformHandle mTexPagesUniform;


    int mDebugDisplay{0};
    bool mDebugGrid{true};
    bool mDebugPageAllocation{true};


    int mCurrentFrame{};
    int mDesiredFrame{1};
};
