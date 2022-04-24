#pragma once
#include <bgfx/bgfx.h>
#include <vector>
#include <assert.h>
struct Buf
{
    Buf(int size, int componentCount)
        : mSize(size)
        , mComponentCount(componentCount)
    {
        mBuffer.resize(size * size * componentCount, 0.f);
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
};

struct CPU
{
    CPU();
    void Init();
    void Tick();
    bgfx::TextureHandle mTexture{bgfx::kInvalidHandle};

    Buf mDensity;
    Buf mVelocity;
    
    bgfx::ProgramHandle m_gradientCSProgram;
    bgfx::FrameBufferHandle m_renderTarget;
};
