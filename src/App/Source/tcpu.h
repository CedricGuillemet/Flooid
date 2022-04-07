#pragma once
#include <bgfx/bgfx.h>
#include <vector>

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
};

struct CPU
{
    CPU();
    void Init();
    void Tick();
    bgfx::TextureHandle mTexture{bgfx::kInvalidHandle};

    Buf mDensity;
    Buf mVelocity;
};
