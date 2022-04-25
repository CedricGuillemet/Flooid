#pragma once
#include <bgfx/bgfx.h>
#include <vector>
#include <assert.h>
#include "TextureProvider.h"

struct Buf
{
    Buf(int size, int componentCount)
        : mSize(size)
        , mComponentCount(componentCount)
    {
        mBuffer.resize(size * size * componentCount, 0.f);

        mTexture = bgfx::createTexture2D(256, 256, false, 1, (componentCount == 1) ? bgfx::TextureFormat::R32F : bgfx::TextureFormat::RGBA32F);
    }

    std::vector<float> mBuffer;
    size_t mSize;
    size_t mComponentCount;

    void Set(float value)
    {
        assert(mComponentCount == 1);
        for (size_t i = 0; i < mBuffer.size(); i++)
        {
            mBuffer[i] = value;
        }
    }

    bgfx::TextureHandle mTexture{ bgfx::kInvalidHandle };
    
    void Upload()
    {
        auto mem = bgfx::makeRef(mBuffer.data(), mBuffer.size() * sizeof(float));//, ReleaseBufFn, &display);
        bgfx::updateTexture2D(mTexture, 0, 0, 0, 0, mSize, mSize, mem);
    }

    void InitRT()
    {
        if (bgfx::isValid(m_renderTarget)) {
            return;
        }
        size_t size = 256;
        const auto texFormat = bgfx::TextureFormat::RGBA32F;
        m_renderTarget = bgfx::createFrameBuffer(size, size, texFormat, BGFX_TEXTURE_COMPUTE_WRITE | BGFX_TEXTURE_RT);
    }
    bgfx::FrameBufferHandle m_renderTarget{ bgfx::kInvalidHandle };
};

struct CPU
{
    CPU();
    void Init();
    void Tick(TextureProvider& textureProvider);
    bgfx::TextureHandle mTexture{bgfx::kInvalidHandle};

    //Buf mDensity;
    //Buf mVelocity;
    
    bgfx::ProgramHandle m_gradientCSProgram;
    //bgfx::FrameBufferHandle m_renderTarget;



    bgfx::UniformHandle m_jacobiParametersUniform;
    bgfx::ProgramHandle m_jacobiCSProgram;


    void Jacobi(TextureProvider& textureProvider, Buf& u, const Buf& rhs, int iterationCount, float hsq);
    void JacobiStep(TextureProvider& textureProvider, const Buf& source, const Buf& rhs, Buf& destination, float hsq);
    void vcycle(TextureProvider& textureProvider, const Buf& rhs, Buf& u, int fineSize, int level, int max);

    bgfx::UniformHandle m_texUUniform;
    bgfx::UniformHandle m_texRHSUniform;
    bgfx::UniformHandle m_invhsqUniform;
};
