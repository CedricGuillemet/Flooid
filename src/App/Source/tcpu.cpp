#include "Immath.h"
#include "tcpu.h"
#include <assert.h>

#define FULL
CPU::CPU()
    : mDensity(256, 1)
    , mVelocity(256, 2)
{}

float length(float x, float y)
{
    return sqrtf(x * x + y * y);
}
void FillDensity(Buf& buf)
{
    assert(buf.mComponentCount == 1);
    auto comp = buf.mComponentCount;

    for (int y = 0; y < buf.mSize; y++)
    {
        for (int x = 0; x < buf.mSize; x++)
        {
            float *p = &buf.mBuffer[(y * buf.mSize + x) * comp];
            float px = x / float(buf.mSize - 1);
            float py = y / float(buf.mSize - 1);

            float npx = 0.5f;
            float npy = 0.1f;
            float dist = length(px - npx, py - npy) - 0.1f;
            float v = (dist<0.f) ? 1.f : 0.f;
            p[0] = v;
        }
    }
}

void FillVelocity(Buf& buf)
{
    assert(buf.mComponentCount == 2);
    auto comp = buf.mComponentCount;
    
    for (int y = 0; y < buf.mSize; y++)
    {
        for (int x = 0; x < buf.mSize; x++)
        {
            float* p = &buf.mBuffer[(y * buf.mSize + x) * comp];
            float px = x / float(buf.mSize - 1);
            float py = y / float(buf.mSize - 1);

            float npx = 0.5f;
            float npy = 0.1f;
            float dist = length(px - npx, py - npy) - 0.1f;
            float v = (dist < 0.f) ? 1.4f : 0.f;
            if (v > FLT_EPSILON)
            {
                p[0] += 0.f;//(float(rand()&255) / 255.f - 0.5f) * 1.f;
                p[1] += v;
            }
        }
    }
}


void Boundary(Buf& velocity)
{
    assert(velocity.mComponentCount == 2);
    auto comp = velocity.mComponentCount;
    for (int y = 0; y < velocity.mSize; y++)
    {
        int index;
        {
            float& v0 = velocity.mBuffer[(y * velocity.mSize + 0) * comp];
            v0 = fabsf(v0);
            float& v1 = velocity.mBuffer[(y * velocity.mSize + 1) * comp];
            v1 = fabsf(v1);
        }
        {
            float& v0 = velocity.mBuffer[(y * velocity.mSize + velocity.mSize - 1) * comp];
            v0 = -fabsf(v0);
            float& v1 = velocity.mBuffer[(y * velocity.mSize + velocity.mSize - 2) * comp];
            v1 = -fabsf(v1);
        }

    }

    for (int x = 0; x < velocity.mSize; x++)
    {
        int index;
        {
            float& v0 = velocity.mBuffer[(0 * velocity.mSize + x) * comp + 1];
            v0 = fabsf(v0);
            float& v1 = velocity.mBuffer[(1 * velocity.mSize + x) * comp + 1];
            v1 = fabsf(v1);
        }
        {
            float& v0 = velocity.mBuffer[((velocity.mSize-1) * velocity.mSize + x) * comp + 1];
            v0 = -fabsf(v0);
            float& v1 = velocity.mBuffer[((velocity.mSize-2) * velocity.mSize + x) * comp + 1];
            v1 = -fabsf(v1);
        }

    }
}

void Divergence(Buf& source, Buf& destination)
{
    assert(source.mComponentCount == 2);
    assert(destination.mComponentCount == 1);
    auto comp = source.mComponentCount;
    for (int y = 1; y < source.mSize-1; y++)
    {
        for (int x = 1; x < source.mSize-1; x++)
        {
            int index = (y * source.mSize + x) * comp;

            float wL = source.mBuffer[index - comp];//texelFetch(s_texVelocity, coord - dx, 0).x;
            float wR = source.mBuffer[index + comp];//texelFetch(s_texVelocity, coord + dx, 0).x;
            float wB = source.mBuffer[index - source.mSize * comp + 1];//texelFetch(s_texVelocity, coord - dy, 0).y;
            float wT = source.mBuffer[index + source.mSize * comp + 1];//texelFetch(s_texVelocity, coord + dy, 0).y;

            float scale = 0.5 / 1.; // 0.5 / gridscale
            float divergence = scale * (wR - wL + wT - wB);
            
            float* pd = &destination.mBuffer[y * source.mSize + x];
            pd[0] = divergence;
        }
    }
}


void Sample(Buf& source, float x, float y, float* values, float damp)
{
    int px = floorf(x);
    int py = floorf(y);
    if (px < 0 || px >= source.mSize-1 || py < 0 || py >= source.mSize-1)
    {
        for (int i = 0; i < source.mComponentCount; i++)
        {
            values[i] = 0.f;
        }
        return;
    }

    float fx = fmodf(x, 1.f);
    float fy = fmodf(y, 1.f);


    int index = (py * source.mSize + px) * source.mComponentCount;
    float* pxy = &source.mBuffer[index];
    float* px1y = &source.mBuffer[index + source.mComponentCount];
    float* pxy1 = &source.mBuffer[index + source.mSize * source.mComponentCount];
    float* px1y1 = &source.mBuffer[index + source.mSize * source.mComponentCount + source.mComponentCount];
    
    for (int i = 0; i < source.mComponentCount; i++)
    {
        values[i] = Imm::Lerp(Imm::Lerp(pxy[i], px1y[i], fx), Imm::Lerp(pxy1[i], px1y1[i], fx), fy) * damp;
    }
}

void Advect(Buf& source, Buf& velocity, Buf& destination)
{
    assert(source.mComponentCount == 2);
    assert(velocity.mComponentCount == 2);
    assert(destination.mComponentCount == 2);
    
    for (int y = 1; y < source.mSize - 1; y++)
    {
        for (int x = 1; x < source.mSize - 1; x++)
        {
            int index = (y * source.mSize + x) * 2;
            float* pd = &destination.mBuffer[index];
            float* pv = &velocity.mBuffer[index];

            const float scale = 1.f;
            float px = x - pv[0] * scale;
            float py = y - pv[1] * scale;

            Sample(source, px, py, pd, 1.f);//0.999f);
        }
    }
}
void Gradient(Buf& pressure, Buf& velocity, Buf& destination)
{
    assert(pressure.mComponentCount == 1);
    assert(velocity.mComponentCount == 2);
    assert(destination.mComponentCount == 2);

    for (int y = 1; y < pressure.mSize - 1; y++)
    {
        for (int x = 1; x < pressure.mSize - 1; x++)
        {
            int index = (y * pressure.mSize + x);
            float* pd = &destination.mBuffer[index * 2];


            float pL = pressure.mBuffer[index - 1];//texelFetch(s_texPressure, coord - dx, 0).x;
            float pR = pressure.mBuffer[index + 1];//texelFetch(s_texPressure, coord + dx, 0).x;
            float pB = pressure.mBuffer[index - pressure.mSize * 1];//texelFetch(s_texPressure, coord - dy, 0).x;
            float pT = pressure.mBuffer[index + pressure.mSize * 1];//texelFetch(s_texPressure, coord + dy, 0).x;

            float scale = 0.5 / 1.; // 0.5 / gridscale
            float gradientx = scale * (pR - pL);
            float gradienty = scale * (pT - pB);

            float* wc = &velocity.mBuffer[index * 2];//texelFetch(s_texVelocity, coord, 0).xy;
            float valuex = wc[0] - gradientx;
            float valuey = wc[1] - gradienty;

            pd[0] = valuex;
            pd[1] = valuey;
        }
    }
}

void JacobiStep(const Buf& source, const Buf& rhs, Buf& destination, float hsq)
{
    assert(source.mComponentCount == 1);
    //assert(divergence.mComponentCount == 1);
    assert(destination.mComponentCount == 1);
    //assert(source.mSize == divergence.mSize);
    assert(source.mSize == destination.mSize);
    
    for (int y = 1; y < source.mSize - 1; y++)
    {
        for (int x = 1; x < source.mSize - 1; x++)
        {
            int index = (y * source.mSize + x);

            const float omega = 4.f / 5.f;
            //float hsq = 1.f / 128.f;
            //hsq *= hsq;
            float value = source.mBuffer[index] + omega * 0.25f * (-hsq * rhs.mBuffer[index] +
                source.mBuffer[index - 1] +
                source.mBuffer[index + 1] +
                source.mBuffer[index - source.mSize] +
                source.mBuffer[index + source.mSize] -
                4.f * source.mBuffer[index]
            );
            float* pd = &destination.mBuffer[index];
            pd[0] = value;
        }
    }
}

void Jacobi(Buf& u, const Buf& rhs, int iterationCount, float hsq)
{
    assert(u.mComponentCount == 1);
    assert(rhs.mComponentCount == 1);

    Buf jacobiBuf(u.mSize, 1);
    
    Buf* jacobis[2] = {&u, &jacobiBuf};
    iterationCount &= ~1;
    for (int i = 0; i < iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;

        JacobiStep(*jacobis[indexSource], rhs, *jacobis[indexDestination], hsq);
    }
}


void coarsen(const Buf& uf, Buf& uc)
{
    assert(uf.mSize == uc.mSize * 2);
    for (int jc = 1; jc < uc.mSize; jc++)
    {
      for (int ic = 1; ic < uc.mSize; ic++)
      {
          int indexDst = jc * uc.mSize + ic;
          int indexSrc = jc * 2 * uf.mSize + ic * 2;
          uc.mBuffer[indexDst] = 0.5 * uf.mBuffer[indexSrc] + 0.25 * (uf.mBuffer[indexSrc - 1] +
                                                                       uf.mBuffer[indexSrc + 1] +
                                                                       uf.mBuffer[indexSrc - uf.mSize] +
                                                                       uf.mBuffer[indexSrc + uf.mSize])
          
          + 0.125 * (uf.mBuffer[indexSrc - 1 - uf.mSize] +
                                                                      uf.mBuffer[indexSrc + 1 - uf.mSize] +
                                                                      uf.mBuffer[indexSrc - 1 + uf.mSize] +
                                                                      uf.mBuffer[indexSrc + 1 + uf.mSize])
          ;
      }
    }
}

void refine_and_add(const Buf& u, Buf& uf)
{
    for (int jc = 1; jc < u.mSize; jc++)
    {
      for (int ic = 1; ic < u.mSize; ic++)
      {
          int indexSrc = jc * u.mSize + ic;
          int indexDst = jc * 2 * uf.mSize + ic * 2;

          int dx =(ic >= u.mSize-1) ? 0 : 1;
          int dy =(jc >= u.mSize-1) ? 0 : 1;
          float v00 = u.mBuffer[indexSrc];
          float v01 = u.mBuffer[indexSrc + dx];
          float v10 = u.mBuffer[indexSrc + dy * u.mSize];
          float v11 = u.mBuffer[indexSrc + dy * u.mSize + dx];
          
          uf.mBuffer[indexDst] += v00;
          uf.mBuffer[indexDst+1] += (v00 + v01) * 0.5;
          uf.mBuffer[indexDst+uf.mSize] += (v00 + v10) * 0.5;
          uf.mBuffer[indexDst+uf.mSize+1] += (v00 + v01 + v10 + v11) * 0.25;
      }
    }
}

void compute_residual(const Buf& u, const Buf& rhs, Buf& res, float hsq)
{
    const float invhsq = 1.f / hsq;
    for (int jc = 1; jc < u.mSize; jc++)
    {
      for (int ic = 1; ic < u.mSize; ic++)
      {
          int index = jc * u.mSize + ic;
          int dx =(ic >= u.mSize-1) ? 0 : 1;
          int dy =(jc >= u.mSize-1) ? 0 : 1;

          res.mBuffer[index] = rhs.mBuffer[index] - (
                                                     4. * u.mBuffer[index]
                                                     - u.mBuffer[index - dx]
                                                     - u.mBuffer[index + dx]
                                                     - u.mBuffer[index - dy * u.mSize]
                                                     - u.mBuffer[index + dy * u.mSize]
                                                     ) * invhsq;
      }
    }
}

void compute_and_coarsen_residual(const Buf& u, const Buf& rhs, Buf& resc, float hsq)
{
    Buf resf(u.mSize, 1);
    resf.Set(0.f);
    compute_residual(u, rhs, resf, hsq);
    coarsen(resf, resc);
}

void CPU::Init()
{
    mTexture = bgfx::createTexture2D(256, 256, false, 1, (true) ? bgfx::TextureFormat::R32F : bgfx::TextureFormat::RGBA32F);


    FillDensity(mDensity);
    (mVelocity);
}

void boundary(Buf& buffer)
{
    float* ptr = buffer.mBuffer.data();
    int n = buffer.mSize;
    for (int i = 0; i < n; i++)
    {
        ptr[i] = 0.f;
        ptr[n*n-1-i] = 0.f;
        ptr[i * n] = 0.f;
        ptr[i * n + n - 1] = 0.f;
    }
}

void vcycle(const Buf& rhs, Buf& u, int fineSize, int level, int max)
{
    int ssteps = 3;
    float hsq = level+1;//sqrtf((level+1)*2);
    
    if (level == max)
    {
        Jacobi(u, rhs, 2, hsq);
        return;
    }
    
    int sizeNext = fineSize / powf(2.f, level+1);
    
    Buf rhsNext(sizeNext, 1);
    Buf uNext(sizeNext, 1);
    
    Jacobi(u, rhs, ssteps, hsq);
    compute_and_coarsen_residual(u, rhs, rhsNext, hsq);
    uNext.Set(0.f);
    
    vcycle(rhsNext, uNext, fineSize, level+1, max);
    
    refine_and_add(uNext, u);

    Jacobi(u, rhs, ssteps, hsq);
}

void CPU::Tick()
{
    Buf advectedVelocity(256, 2);
    Advect(mVelocity, mVelocity, advectedVelocity);
    mVelocity.mBuffer = advectedVelocity.mBuffer;

    //Boundary(mVelocity);
    FillVelocity(mVelocity);
    
    //Buf* divergence = new Buf(256, 1);
    Buf divergence(256, 1);

    //Buf* divergence = &divergence_;
    Divergence(mVelocity, divergence);
    boundary(divergence);
    /// <summary>
    /// 
    /// </summary>
    

    // good 1
    /*
    Buf newPressure(256, 1);
    Buf rhs(256, 1);
    Jacobi(newPressure, divergence, 50);
*/
    
    Buf u(256, 1);
    vcycle(divergence, u, u.mSize, 0, 0);
    
    ///
    Buf newVelocity(256, 2);
    Gradient(u, mVelocity, newVelocity);

    mVelocity.mBuffer = newVelocity.mBuffer;

    

/*    if (bgfx::isValid(mTexture))
    {
        bgfx::destroy(mTexture);
    }
  */
    static Buf display(256,1);
    display.mBuffer = u.mBuffer;

    
    auto mem = bgfx::makeRef(display.mBuffer.data(), display.mBuffer.size() * sizeof(float));//, ReleaseBufFn, &display);
    bgfx::updateTexture2D(mTexture, 0,0,0,0, display.mSize, display.mSize,mem);
}
