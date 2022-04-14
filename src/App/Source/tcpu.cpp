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
            if (fabsf(pv[0]) > FLT_EPSILON || fabsf(pv[1]) > FLT_EPSILON)
            {
                int a = 1;
            }
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

    float* src = bufferU.mBuffer.data();
    float* dst = bufferUf.mBuffer.data();
    
    int nSrc = bufferU.mSize;
    int nDst = bufferUf.mSize;
    
    for(int j = 0; j < nSrc-1; j++)
    {
        int ofsSrc = (j * nSrc);
        int ofsDst = (j * nDst * 2);
        for (int i = 0; i < nSrc-1; i++)
        {
            float horz = 0.5 * (src[ofsSrc + i] + src[ofsSrc + 1]);
            float vert = 0.5 * (src[ofsSrc + i] + src[ofsSrc + nSrc]);

            dst[ofsDst + i * 2] += src[ofsSrc + i];
            dst[ofsDst + i * 2 + 1] += horz;

            dst[ofsDst + nDst + i * 2] += vert;
            dst[ofsDst + nDst + i * 2 + 1] += 0.25 * (src[ofsSrc + i] + src[ofsSrc + i + 1] + src[ofsSrc + i + nSrc] + src[ofsSrc + i + nSrc + 1]);
        }
    }
}

void jacobi2(Buf& bufferU, Buf& bufferRhs, float hsq, int ssteps)
{
    /* Jacobi damping parameter -- plays an important role in MG */
    float omega = 4.f / 5.f;
    Buf bufferUNew(bufferU.mSize, 1);
    
    int N = bufferU.mSize;
    bufferUNew.Set(0.f);
    
    for (int step = 0; step < ssteps; step++) {
        float* unew = bufferUNew.mBuffer.data();//(double*)malloc(sizeof(double) * (N + 1));
        float* rhs = bufferRhs.mBuffer.data();//(double*)malloc(sizeof(double) * (N + 1));
        float* u = bufferU.mBuffer.data();
        for (int j = 1; j < N - 1; j++) {
            for (int i = 1; i < N - 1; i++) {
                int idx = j * N + i;
                unew[idx] = u[idx] + omega * 0.25f * (hsq * rhs[idx] + u[idx - 1] + u[idx + 1] + u[idx - N] + u[idx + N] - 4.f * u[idx]);
                //unew[idx] = u[idx] + omega * 0.5 * (/*hsq * rhs[idx] +*/ u[idx - 1] + u[idx + 1] - 2.f * u[idx]);
                //unew[idx] = u[idx] + omega * 0.25f * (hsq * rhs[i] + u[idx - 1] + u[idx + 1] + u[idx - N] + u[idx + N] - 4.f * u[idx]);
            }
        }
        //memcpy(u, unew, (N + 1) * sizeof(double));
        bufferU.mBuffer = bufferUNew.mBuffer;
    }
    //free(unew);
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


/* set vector to zero */
void set_zero(double* u, int N) {
    int i;
    for (i = 0; i < (N + 1) * (N + 1); i++)
        u[i] = 0.0;
}

/* coarsen uf from length N+1 to lenght N/2+1
   assuming N = 2^l
*/
void coarsen(double* uf, double* uc, int N) {
    int ic, jc;
    memset(uc, 0, sizeof(double) * (N/2 + 1) * (N/2 + 1));
    for (jc = 1; jc < N / 2; ++jc)
    {
        for (ic = 1; ic < N / 2; ++ic)
        {
            int index = jc * (N + 1) + ic * 2;
            int dst = jc * (N /2 + 1) + ic;
            uc[dst] = 0.5 * uf[index] + 0.25 * (uf[index - 1] + uf[index + 1] + uf[index - N - 1] + uf[index + N + 1]);
        }
    }
}


/* refine u from length N+1 to lenght 2*N+1
   assuming N = 2^l, and add to existing uf
*/
void refine_and_add(double* u, double* uf, int N)
{
    int i, j;
    //uf[1] += 0.5 * (u[0] + u[1]);
    for (j = 1; j < N; j++)
    {
        for (i = 1; i < N; ++i)
        {
            int dst = j * (N + 1) * 2 + i * 2;
            int src = j * (N + 1) + i;
            uf[dst] += u[src];
            uf[dst + 1] += 0.5 * (u[src] + u[src + 1]);
            uf[dst + N*2 + 1] += 0.5 * (u[src] + u[src + N + 1]);

            uf[dst + N*2 + 1 + 1] += 0.25 * (u[src] + u[src + N + 1] + u[src + 1] + u[src + N + 1 + 1]);
        }
    }
}

/* compute residual vector */
void compute_residual(double* u, double* rhs, double* res, int N, double invhsq)
{
    int i, j;
    for (j = 1; j < N; j++)
    {
        for (i = 1; i < N; i++)
        {
            int index = j * (N + 1) + i;
            res[index] = (rhs[index] - (4. * u[index] - u[index - 1] - u[index + 1] - u[index - N - 1] - u[index + N + 1]) * invhsq);
        }
    }

}


/* compute residual and coarsen */
void compute_and_coarsen_residual(double* u, double* rhs, double* resc,
    int N, double invhsq)
{
    size_t memsize = sizeof(double) * (N + 1) * (N + 1);
    double* resf = (double*)malloc(memsize);
    memset(resf, 0, memsize);
    compute_residual(u, rhs, resf, N, invhsq);
    coarsen(resf, resc, N);
    free(resf);
}


void test_compute_and_coarsen_residual()
{
    size_t N = 32;
    size_t count = (N + 1) * (N + 1);
    size_t half = (N/2 + 1) * (N/2 + 1);
    
    double* res = new double[half];
    memset(res, 0, sizeof(double) * half);

    double* u = new double[count];
    memset(u, 0, sizeof(double) * count);

    double* rhs = new double[count];
    memset(rhs, 0, sizeof(double) * count);

    for (int j = 1; j < N; j++)
    {
        for (int i = 1; i < N; i++)
        {
            u[j * (N+1) + i] = j * 100;// + i;
            rhs[j * (N + 1) + i] = 0.;
        }
    }

    compute_and_coarsen_residual(u, rhs, res, N, 1.f/256.f);

    delete [] res;
    delete [] u;
    delete [] rhs;
}

/* Perform Jacobi iterations on u */
void jacobi(double* u, double* rhs, int N, double hsq, int ssteps)
{
    int i, j, k;
    /* Jacobi damping parameter -- plays an important role in MG */
    double omega = 4. / 5.;
    double* unew = (double*)malloc(sizeof(double) * (N + 1) * (N + 1));
    for (i = 0; i < (N + 1) * (N + 1); ++i) {
        unew[i] = 0.;
    }
    for (j = 0; j < ssteps; ++j) {
        for (k = 1; k < N; k++) {
            for (i = 1; i < N; i++) {
                int idx = k * (N+1) + i;
                unew[idx] = u[idx] + omega * 0.25 * (hsq * rhs[idx] + u[idx - 1] + u[idx + 1] + u[idx - N - 1] + u[idx + N + 1] - 4 * u[idx]);
            }
        }
        memcpy(u, unew, (N + 1) * (N + 1) * sizeof(double));
    }
    free(unew);
}


void doit(Buf& mybuf)
{
    int i, j, Nfine, l, iter, max_iters, levels, ssteps = 3;
    /*
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: ./multigrid_1d Nfine maxiter [s-steps]\n");
        fprintf(stderr, "Nfine: # of intervals, must be power of two number\n");
        fprintf(stderr, "s-steps: # jacobi smoothing steps (optional, default is 3)\n");
        abort();
    }
    sscanf(argv[1], "%d", &Nfine);
    sscanf(argv[2], "%d", &max_iters);
    if (argc > 3)
        sscanf(argv[3], "%d", &ssteps);
        */

    Nfine = 256;
    max_iters = 1;
    ssteps = 1;

    /* compute number of multigrid levels */
    levels = floor(log2(Nfine));
    printf("Multigrid Solve using V-cycles for -u'' = f on (0,1)\n");
    printf("Number of intervals = %d, max_iters = %d\n", Nfine, max_iters);
    printf("Number of MG levels: %d \n", levels);

    

    /* Allocation of vectors, including left and right bdry points */
    double** u = new double*[levels], ** rhs = new double*[levels];
    /* N, h*h and 1/(h*h) on each level */
    int* N = (int*)malloc(sizeof(int) * levels);
    double* invhsq = (double*)malloc(sizeof(double) * levels);
    double* hsq = (double*)malloc(sizeof(double) * levels);
    double* res = (double*)malloc(sizeof(double) * (Nfine + 1));
    for (l = 0; l < levels; ++l) {
        N[l] = Nfine / (int)pow(2, l);
        double h = 1.0 / N[l];
        hsq[l] = h * h;
        printf("MG level %2d, N = %8d\n", l, N[l]);
        invhsq[l] = 1.0 / hsq[l];
        u[l] = (double*)malloc(sizeof(double) * (N[l] + 1) * (N[l] + 1));
        for (int i = 0; i < N[l] + 1; ++i) {
            u[l][i] = 0.;
        }
        rhs[l] = (double*)malloc(sizeof(double) * (N[l] + 1) * (N[l] + 1));
    }
    /* rhs on finest mesh */
    for (i = 0; i <= N[0] * (N[0]+1); ++i) {
        rhs[0][i] = 1.0;
    }

    for (j = 0; j <= N[0]; ++j) {
        for (i = 0; i <= N[0]; ++i) {
            u[0][j*(N[0] +1) + i] = 0.f;//mybuf.mBuffer[j * 256 + i];
        }
    }

    for (j = 0; j < N[0]; ++j) {
        for (i = 0; i < N[0]; ++i) {
            u[0][j * (N[0] + 1) + i] = mybuf.mBuffer[j * 256 + i];
        }
    }


    /* set boundary values (unnecessary if calloc is used) */
    //u[0][0] = u[0][N[0]] = 0.0;

    for (iter = 0; iter < max_iters; iter++) {
        /* V-cycle: Coarsening */
        for (l = 0; l < levels - 1; ++l) {
            /* pre-smoothing and coarsen */
            jacobi(u[l], rhs[l], N[l], hsq[l], ssteps);
            compute_and_coarsen_residual(u[l], rhs[l], rhs[l + 1], N[l], invhsq[l]);
            /* initialize correction for solution with zero */
            set_zero(u[l + 1], N[l + 1]);
        }
        /* V-cycle: Solve on coarsest grid using many smoothing steps */
        jacobi(u[levels - 1], rhs[levels - 1], N[levels - 1], hsq[levels - 1], 50);

        /* V-cycle: Refine and correct */
        for (l = levels - 1; l > 0; --l) {
            /* refine and add to u */
            refine_and_add(u[l], u[l - 1], N[l]);
            /* post-smoothing steps */
            jacobi(u[l - 1], rhs[l - 1], N[l - 1], hsq[l - 1], ssteps);
        }
    }

    for (j = 0; j < N[0]; ++j) {
        for (i = 0; i < N[0]; ++i) {
            mybuf.mBuffer[j * 256 + i] = u[0][j * (N[0] + 1) + i];
        }
    }

    /* Clean up */
    free(hsq);
    free(invhsq);
    free(N);
    free(res);
    for (l = levels - 1; l >= 0; --l) {
        free(u[l]);
        free(rhs[l]);
    }
}

void CPU::Tick()
{
    test_compute_and_coarsen_residual();

    Buf advectedVelocity(256, 2);
    Advect(mVelocity, mVelocity, advectedVelocity);
    mVelocity.mBuffer = advectedVelocity.mBuffer;

    //Boundary(mVelocity);
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
    int postSmoothIteration = 1;
    int smoothIteration = 1;
    /////////////////////////////////////////////////////////////////////////////////////////


    doit(divergence);
#if 0
    Buf u(256, 1);
    Buf u2(128, 1);

    Buf rhs(256, 1);
    rhs.Set(1.f);
    Buf rhs2(128, 1);

    
    float h = 1.f / 256.f;
    float h2 = 1.f / 128.f;
    float hsq = h * h;
    float hsq2 = h2 * h2;
    float invsq = 1.f / hsq;
#if 0
    jacobi2(u, rhs, hsq, preSmoothIteration);
    compute_and_coarsen_residual(u, rhs, rhs2, invsq);


    jacobi2(u2, rhs2, hsq2, smoothIteration);


    refine_and_add(u2, u);
    /* post-smoothing steps */
    jacobi2(u, rhs, hsq, postSmoothIteration);
#endif
    boundary(divergence);
    //jacobi2(divergence, rhs, hsq, 1000);

    /// <summary>
    /// //
    /// </summary>
    Buf newPressure(256, 1);
    newPressure.mBuffer = divergence.mBuffer;
#endif
    /////////////////////////////////////////////////////////////////////////////////////////

    Buf newVelocity(256, 2);
    Gradient(divergence, mVelocity, newVelocity);

    mVelocity.mBuffer = newVelocity.mBuffer;

    

/*    if (bgfx::isValid(mTexture))
    {
        bgfx::destroy(mTexture);
    }
  */
    static Buf display(256,1);
    display.mBuffer = divergence.mBuffer;

    
    auto mem = bgfx::makeRef(display.mBuffer.data(), display.mBuffer.size() * sizeof(float));//, ReleaseBufFn, &display);
    bgfx::updateTexture2D(mTexture, 0,0,0,0, display.mSize, display.mSize,mem);
}
