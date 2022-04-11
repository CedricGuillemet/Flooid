#include "tcpu.h"
#include "Immath.h"
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
            float v = (dist < 0.f) ? 0.4f : 0.f;
            if (v > FLT_EPSILON)
            {
                p[0] = 0;
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

            const float scale = 0.1f;
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

void JacobiStep(Buf& source, Buf& divergence, Buf& rhs, Buf& destination)
{
    assert(source.mComponentCount == 1);
    assert(divergence.mComponentCount == 1);
    assert(destination.mComponentCount == 1);
    assert(source.mSize == divergence.mSize);
    assert(source.mSize == destination.mSize);
    
    for (int y = 1; y < source.mSize - 1; y++)
    {
        for (int x = 1; x < source.mSize - 1; x++)
        {
            int index = (y * source.mSize + x);


            float xL = source.mBuffer[index - 1];//texelFetch(s_texJacobi, coord - dx, 0).x;
            float xR = source.mBuffer[index + 1];//texelFetch(s_texJacobi, coord + dx, 0).x;
            float xB = source.mBuffer[index - source.mSize];//texelFetch(s_texJacobi, coord - dy, 0).x;
            float xT = source.mBuffer[index + source.mSize];//texelFetch(s_texJacobi, coord + dy, 0).x;

            float bC = divergence.mBuffer[index]; //texelFetch(s_texDivergence, coord, 0).x;

            float jacobiParametersx = -1.f;
            float jacobiParametersy = 4.f;
            float value = (xL + xR + xB + xT + jacobiParametersx * bC) / jacobiParametersy;

            float* pd = &destination.mBuffer[index];
            pd[0] = value;
        }
    }
}

void Jacobi(Buf& divergence, Buf& rhs, Buf& destination, int iterationCount)
{
    assert(divergence.mComponentCount == 1);
    assert(destination.mComponentCount == 1);

    Buf jacobi0(destination.mSize, 1);
    Buf jacobi1(destination.mSize, 1);

    Buf* jacobi[2] = {&jacobi0, &jacobi1};
    for (int i = 0; i < iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;

        JacobiStep(*jacobi[indexSource], divergence, rhs, *jacobi[indexDestination]);
    }
    const int lastJacobiIndex = iterationCount & 1;
    destination.mBuffer = jacobi[lastJacobiIndex]->mBuffer;
}

void CPU::Init()
{
    mTexture = bgfx::createTexture2D(256, 256, false, 1, (true) ? bgfx::TextureFormat::R32F : bgfx::TextureFormat::RGBA32F);


    FillDensity(mDensity);
    FillVelocity(mVelocity);
}

void ReleaseBufFn(void* _ptr, void* _userData)
{
    Buf* buf = (Buf*)_userData;
    delete buf;
}


void prolongate2D(const Buf& source, Buf& destination)
{
    assert(source.mComponentCount == destination.mComponentCount);
    assert(source.mComponentCount == 1);
    assert(source.mSize *2 == destination.mSize);
    
    float* uf = destination.mBuffer.data();
    const float* uc = source.mBuffer.data();

    /* Prolongate from coarse to fine mesh.
     * The coarse mesh has 2^level + 1 points.
     * The fine mesh has 2^(level+1) + 1 points.
     * Uses bilinear interpolation.
     */
    int nc, nf;
    int ic, jc, iif, jf;
    int indx;

    nc = source.mSize;
    nf = destination.mSize;
    //nc += 1;
    //nf += 1;


/* Copy values at matching nodes */
    for(ic=0; ic < nc; ic++) {
        for(jc = 0; jc < nc; jc++) {
            uf[2*jc + 2*ic*nf] = uc[jc + nc*ic];
        }
    }

/* Odd # columns horizontal interpolation */

    for(iif = 1; iif < nf -1 ; iif += 2) {
        for(jf=2; jf < nf; jf +=2) {
            indx = jf + iif*nf;
            uf[indx] = .5*( uf[indx - nf] + uf[indx + nf]);
        }
    }

/* Even # colums vertical interpolation */

    for(iif=2; iif < nf; iif +=2) {
        for(jf=1;jf<nf-1;jf+=2) {
            indx = jf +iif*nf;
            uf[indx] = .5*(uf[indx-1] + uf[indx+1]);
        }
    }

/* Do corner points if FULL weighting is selected */
    for(iif = 1; iif < nf -1 ; iif +=2 ) {
        for(jf = 1; jf < nf-1;jf+=2) {
            indx = jf+iif*nf;
            uf[indx] = .25*(uf[indx+nf] + uf[indx+1] + uf[indx-nf] + uf[indx-1]);
        }
    }
}

void addint(const Buf& source, Buf& destination) //double *uf, const double *uc, double *res, const int level)
{
    /* Prolongates solution from course mesh to fine mesh
     * and adds that to fine mesh solution
     * level is the course level, where there are 2^level + 1 points
     * res is used for temp storage
     */

    assert(destination.mSize == source.mSize *2);
    
    Buf temp(destination.mSize, destination.mComponentCount);
    float* uf = destination.mBuffer.data();
    float* res = temp.mBuffer.data();
    int i;
    int n = temp.mSize;//1 + (2 << (level+1));
    prolongate2D(source, temp);//res,uc,level);
    for(i=0;i<n*n;i++) {
        uf[i] += res[i];
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

void compute_residual(const Buf& bufferU, const Buf& bufferRhs, Buf& bufferRes, float invhsq)
{
    assert(bufferU.mSize == bufferRhs.mSize);
    assert(bufferU.mSize == bufferRes.mSize);
    assert(bufferU.mComponentCount == bufferRhs.mComponentCount);
    /* Computes the negative residual for the poisson problem. */
    int i, j, indx, n;

    float* res = bufferRes.mBuffer.data();
    const float* rhs = bufferRhs.mBuffer.data();
    const float* u = bufferU.mBuffer.data();

    n = bufferRes.mSize;

    for (i = 1; i < n - 1; i++) {
        for (j = 1; j < n - 1; j++) {
            indx = j + n * i;

            res[indx] = (rhs[indx] - (4. * u[indx] - u[indx - 1] - u[indx + 1] - u[indx + n] - u[indx -n]) * invhsq);
        }
    }
    return;
}


void coarsen(const Buf& source, Buf& destination)
{
    assert(source.mComponentCount == destination.mComponentCount);
    assert(source.mComponentCount == 1);
    assert(source.mSize == destination.mSize * 2);

    float* uc = destination.mBuffer.data();
    const float* uf = source.mBuffer.data();
    /* Restrict from fine grid to coarse grid.
     * Uses half weighting.
     * Fine grid has 2^level + 1 points.
     * Coarse grid has 2^(level-1) + 1 points
     */

    int nc, nf;
    int ic, jc, iif, jf;
    int i, j;
    int indx;

    nf = source.mSize;
    nc = destination.mSize;


    /* Interior points */
    for (i = 1; i < nc - 1; i++) {
        for (j = 1; j < nc - 1; j++) {
            indx = 2 * j + 2 * i * nf;

            uc[j + nc * i] = .25 * uf[indx]
                + .125 * (uf[indx + 1]
                    + uf[indx - 1]
                    + uf[indx + nf]
                    + uf[indx - nf])
                + .0625 * (uf[indx + 1 + nf]
                    + uf[indx + 1 - nf]
                    + uf[indx - 1 + nf]
                    + uf[indx - 1 - nf]);
        }
    }

    /* Boundary Points */

    for (iif = 0; iif < nf; iif += nf - 1) {
        for (jf = 0; jf < nf; jf += 2) {
            ic = iif >> 1;
            jc = jf >> 1;
            uc[jc + nc * ic] = uf[jf + iif * nf];
        }
    }

    for (iif = 1; iif < nf - 1; iif += 2) {
        for (jf = 0; jf < nf; jf += nf - 1) {
            ic = iif >> 1;
            jc = jf >> 1;
            uc[jc + nc * ic] = uf[jf + nf * iif];
        }
    }
}

void compute_and_coarsen_residual(const Buf& bufferU, const Buf& bufferRhs, Buf& bufferRes, double invhsq)
{
    Buf temp(bufferU.mSize, 1);

    compute_residual(bufferU, bufferRhs, temp, invhsq);
    coarsen(temp, bufferRes);
}


void refine_and_add(Buf& bufferU, Buf& bufferUf)
{
    assert(bufferU.mComponentCount == bufferUf.mComponentCount);
    assert(bufferU.mComponentCount == 1);
    assert(bufferU.mSize * 2 == bufferUf.mSize);

    float* u = bufferU.mBuffer.data();
    float* uf = bufferUf.mBuffer.data();
    
    int n = bufferU.mSize;
    
    for(int j = 0;j <n;j++)
    {
        int lnOfs = (j * 2 * n);
        uf[1] += 0.5 * (u[0] + u[1]);
        for (int i = 1; i < n; ++i)
        {
            uf[lnOfs + 2 * i] += u[i];
            uf[lnOfs + 2 * i + 1] += 0.5 * (u[i] + u[i + 1]);
        }
    }
}

void CPU::Tick()
{
    Buf advectedVelocity(256, 2);
    Advect(mVelocity, mVelocity, advectedVelocity);
    mVelocity.mBuffer = advectedVelocity.mBuffer;

    Boundary(mVelocity);
    FillVelocity(mVelocity);
    //Buf* divergence = new Buf(256, 1);
    Buf divergence(256, 1);

    //Buf* divergence = &divergence_;
    Divergence(mVelocity, divergence);

    /// <summary>
    /// 
    /// </summary>
    

    /*
    Buf newPressure(256, 1);
    Jacobi(divergence, nullptr, newPressure, 50);
    */
    int preSmoothIteration = 1;
    int postSmoothIteration = 50;
    int smoothIteration = 1;
    /////////////////////////////////////////////////////////////////////////////////////////
    Buf newPressure(256, 1);
    Buf rhs(256, 1);
    Jacobi(divergence, rhs, newPressure, postSmoothIteration);
    /////////////////////////////////////////////////////////////////////////////////////////

    Buf newVelocity(256, 2);
    Gradient(newPressure, mVelocity, newVelocity);

    mVelocity.mBuffer = newVelocity.mBuffer;

    

/*    if (bgfx::isValid(mTexture))
    {
        bgfx::destroy(mTexture);
    }
  */
    static Buf display(256,1);
    display.mBuffer = newPressure.mBuffer;

    
    auto mem = bgfx::makeRef(display.mBuffer.data(), display.mBuffer.size() * sizeof(float));//, ReleaseBufFn, &display);
    bgfx::updateTexture2D(mTexture, 0,0,0,0, display.mSize, display.mSize,mem);
}
