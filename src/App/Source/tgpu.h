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
    
    
    
    //
    void DensityGen(TextureProvider& textureProvider);
    void VelocityGen(TextureProvider& textureProvider);
    
    void Advect(TextureProvider& textureProvider, Texture* source, Texture* velocity, Texture* output);
    void Gradient(TextureProvider& textureProvider, Texture* u, Texture* velocity, Texture* destination);
    void Divergence(TextureProvider& textureProvider, Texture* velocity, Texture* destination);
    void Jacobi(TextureProvider& textureProvider, Texture* jacobi[2], Texture* rhs, int iterationCount);
};
