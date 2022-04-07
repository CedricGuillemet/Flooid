#include "tcpu.h"
#include "Immath.h"

CPU::CPU()
    : mDensity(256)
    , mVelocity(256)
{}

float length(float x, float y)
{
    return sqrtf(x * x + y * y);
}
void FillDensity(Buf& buf)
{
    for (int y = 0; y < buf.mSize; y++)
    {
        for (int x = 0; x < buf.mSize; x++)
        {
            float *p = &buf.mBuffer[(y * buf.mSize + x) * 4];
            float px = x / float(buf.mSize - 1);
            float py = y / float(buf.mSize - 1);

            float npx = 0.5f;
            float npy = 0.1f;
            float dist = length(px - npx, py - npy) - 0.1f;
            float v = (dist<0.f) ? 1.f : 0.f;
            p[0] = v;
            p[1] = v;
            p[2] = v;
            p[3] = v;
        }
    }
}

void FillVelocity(Buf& buf)
{
    for (int y = 0; y < buf.mSize; y++)
    {
        for (int x = 0; x < buf.mSize; x++)
        {
            float* p = &buf.mBuffer[(y * buf.mSize + x) * 4];
            float px = x / float(buf.mSize - 1);
            float py = y / float(buf.mSize - 1);

            float npx = 0.5f;
            float npy = 0.1f;
            float dist = length(px - npx, py - npy) - 0.1f;
            float v = (dist < 0.f) ? 0.4f : 0.f;
            if (v > FLT_EPSILON)
            {
                p[0] = 0;
                p[1] += v;
                p[2] = 0;
                p[3] = 0;
            }
        }
    }
}


void Boundary(Buf& velocity)
{
    for (int y = 0; y < velocity.mSize; y++)
    {
        int index;
        {
            float& v0 = velocity.mBuffer[(y * velocity.mSize + 0) * 4];
            v0 = fabsf(v0);
            float& v1 = velocity.mBuffer[(y * velocity.mSize + 1) * 4];
            v1 = fabsf(v1);
        }
        {
            float& v0 = velocity.mBuffer[(y * velocity.mSize + velocity.mSize - 1) * 4];
            v0 = -fabsf(v0);
            float& v1 = velocity.mBuffer[(y * velocity.mSize + velocity.mSize - 2) * 4];
            v1 = -fabsf(v1);
        }

    }

    for (int x = 0; x < velocity.mSize; x++)
    {
        int index;
        {
            float& v0 = velocity.mBuffer[(0 * velocity.mSize + x) * 4 +1];
            v0 = fabsf(v0);
            float& v1 = velocity.mBuffer[(1 * velocity.mSize + x) * 4 +1];
            v1 = fabsf(v1);
        }
        {
            float& v0 = velocity.mBuffer[((velocity.mSize-1) * velocity.mSize + x) * 4 +1];
            v0 = -fabsf(v0);
            float& v1 = velocity.mBuffer[((velocity.mSize-2) * velocity.mSize + x) * 4 + 1];
            v1 = -fabsf(v1);
        }

    }
}

void Divergence(Buf& source, Buf& destination)
{
    for (int y = 1; y < source.mSize-1; y++)
    {
        for (int x = 1; x < source.mSize-1; x++)
        {
            int index = (y * source.mSize + x) * 4;
            float* pd = &destination.mBuffer[index];
            

            float wL = source.mBuffer[index - 4];//texelFetch(s_texVelocity, coord - dx, 0).x;
            float wR = source.mBuffer[index + 4];//texelFetch(s_texVelocity, coord + dx, 0).x;
            float wB = source.mBuffer[index - source.mSize * 4 + 1];//texelFetch(s_texVelocity, coord - dy, 0).y;
            float wT = source.mBuffer[index + source.mSize * 4 + 1];//texelFetch(s_texVelocity, coord + dy, 0).y;

            float scale = 0.5 / 1.; // 0.5 / gridscale
            float divergence = scale * (wR - wL + wT - wB);
            pd[0] = divergence;
            pd[1] = 0.f;
            pd[2] = 0.f;
            pd[3] = 1.f;
        }
    }
}


void Sample(Buf& source, float x, float y, float* values, float damp)
{
    int px = floorf(x);
    int py = floorf(y);
    if (px < 0 || px >= source.mSize || py < 0 || py >= source.mSize)
    {
        for (int i = 0; i < 4; i++)
        {
            values[i] = 0.f;
        }
        return;
    }

    float fx = fmodf(x, 1.f);
    float fy = fmodf(y, 1.f);


    int index = (py * source.mSize + px) * 4;
    float* pxy = &source.mBuffer[index];
    float* px1y = &source.mBuffer[index + 4];
    float* pxy1 = &source.mBuffer[index + source.mSize * 4];
    float* px1y1 = &source.mBuffer[index + source.mSize * 4 + 4];
    
    for (int i = 0; i < 4; i++)
    {
        values[i] = Imm::Lerp(Imm::Lerp(pxy[i], px1y[i], fx), Imm::Lerp(pxy1[i], px1y1[i], fx), fy) * damp;
    }
}

void Advect(Buf& source, Buf& velocity, Buf& destination)
{
    for (int y = 1; y < source.mSize - 1; y++)
    {
        for (int x = 1; x < source.mSize - 1; x++)
        {
            int index = (y * source.mSize + x) * 4;
            float* pd = &destination.mBuffer[index];
            float* pv = &velocity.mBuffer[index];

            float px = x - pv[0];
            float py = y - pv[1];

            Sample(source, px, py, pd, 1.f);//0.999f);
        }
    }
}
void Gradient(Buf& pressure, Buf& velocity, Buf& destination)
{
    for (int y = 1; y < pressure.mSize - 1; y++)
    {
        for (int x = 1; x < pressure.mSize - 1; x++)
        {
            int index = (y * pressure.mSize + x) * 4;
            float* pd = &destination.mBuffer[index];


            float pL = pressure.mBuffer[index - 4];//texelFetch(s_texPressure, coord - dx, 0).x;
            float pR = pressure.mBuffer[index + 4];//texelFetch(s_texPressure, coord + dx, 0).x;
            float pB = pressure.mBuffer[index - pressure.mSize * 4];//texelFetch(s_texPressure, coord - dy, 0).x;
            float pT = pressure.mBuffer[index + pressure.mSize * 4];//texelFetch(s_texPressure, coord + dy, 0).x;

            float scale = 0.5 / 1.; // 0.5 / gridscale
            float gradientx = scale * (pR - pL);
            float gradienty = scale * (pT - pB);

            float* wc = &velocity.mBuffer[index];//texelFetch(s_texVelocity, coord, 0).xy;
            float valuex = wc[0] - gradientx;
            float valuey = wc[1] - gradienty;

            pd[0] = valuex;
            pd[1] = valuey;
            pd[2] = 0.f;
            pd[3] = 1.f;
        }
    }
}

void JacobiStep(Buf& source, Buf& divergence, Buf& destination)
{
    for (int y = 1; y < source.mSize - 1; y++)
    {
        for (int x = 1; x < source.mSize - 1; x++)
        {
            int index = (y * source.mSize + x) * 4;


            float xL = source.mBuffer[index - 4];//texelFetch(s_texJacobi, coord - dx, 0).x;
            float xR = source.mBuffer[index + 4];//texelFetch(s_texJacobi, coord + dx, 0).x;
            float xB = source.mBuffer[index - source.mSize * 4];//texelFetch(s_texJacobi, coord - dy, 0).x;
            float xT = source.mBuffer[index + source.mSize * 4];//texelFetch(s_texJacobi, coord + dy, 0).x;

            float bC = divergence.mBuffer[index]; //texelFetch(s_texDivergence, coord, 0).x;

            float jacobiParametersx = -1.f;
            float jacobiParametersy = 4.f;
            float value = (xL + xR + xB + xT + jacobiParametersx * bC) / jacobiParametersy;

            float* pd = &destination.mBuffer[index];
            pd[0] = value;
        }
    }
}

void Jacobi(Buf& divergence, Buf& destination)
{
    Buf jacobi0(256);
    Buf jacobi1(256);

    Buf* jacobi[2] = {&jacobi0, &jacobi1};
    int m_iterationCount = 100;
    for (int i = 0; i < m_iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;

        JacobiStep(*jacobi[indexSource], divergence, *jacobi[indexDestination]);
    }
    const int lastJacobiIndex = m_iterationCount & 1;
    destination.mBuffer = jacobi[lastJacobiIndex]->mBuffer;
}

void CPU::Init()
{
    mTexture = bgfx::createTexture2D(256, 256, false, 1, bgfx::TextureFormat::RGBA32F);

    FillDensity(mDensity);
    FillVelocity(mVelocity);
}

void ReleaseBufFn(void* _ptr, void* _userData)
{
    Buf* buf = (Buf*)_userData;
    delete buf;
}

void CPU::Tick()
{
    Buf advectedVelocity(256);
    Advect(mVelocity, mVelocity, advectedVelocity);
    mVelocity.mBuffer = advectedVelocity.mBuffer;

    Boundary(mVelocity);
    FillVelocity(mVelocity);
    Buf* divergence = new Buf(256);
    Divergence(mVelocity, *divergence);

    Buf newPressure(256);
    Jacobi(*divergence, newPressure);

    Buf newVelocity(256);
    Gradient(newPressure, mVelocity, newVelocity);

    mVelocity.mBuffer = newVelocity.mBuffer;


    Buf& display = *divergence;
    auto mem = bgfx::makeRef(display.mBuffer.data(), display.mBuffer.size() * sizeof(float), ReleaseBufFn, divergence);
    bgfx::updateTexture2D(mTexture, 0,0,0,0, display.mSize, display.mSize,mem);
}