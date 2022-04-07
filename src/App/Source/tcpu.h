#pragma once
#include <bgfx/bgfx.h>
#include <vector>

struct Buf
{
    Buf(int size)
        : mSize(size)
    {
        mBuffer.resize(size * size * 4, 0.f);
    }

    std::vector<float> mBuffer;
    size_t mSize;
};

struct CPU
{
    CPU();
    void Init();
    void Tick();
    bgfx::TextureHandle mTexture;

    Buf mDensity;
    Buf mVelocity;
};