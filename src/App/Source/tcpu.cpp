#include "tcpu.h"
#include "Immath.h"

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
    if (px < 0 || px >= source.mSize || py < 0 || py >= source.mSize)
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

            float px = x - pv[0];
            float py = y - pv[1];

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

void JacobiStep(Buf& source, Buf& divergence, Buf& destination)
{
    assert(source.mComponentCount == 1);
    assert(divergence.mComponentCount == 1);
    assert(destination.mComponentCount == 1);
    
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

void Jacobi(Buf& divergence, Buf& destination)
{
    assert(divergence.mComponentCount == 1);
    assert(destination.mComponentCount == 1);
    
    Buf jacobi0(256, 1);
    Buf jacobi1(256, 1);

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
    mTexture = bgfx::createTexture2D(128, 128, false, 1, (true) ? bgfx::TextureFormat::R32F : bgfx::TextureFormat::RGBA32F);


    FillDensity(mDensity);
    FillVelocity(mVelocity);
}

void ReleaseBufFn(void* _ptr, void* _userData)
{
    Buf* buf = (Buf*)_userData;
    delete buf;
}

void matfill(Buf* source, Buf& destination) //double *u, const double *fill, const int n) {
{
    /* Fill u with the values in fill array.
     * If fill is NULL, then fill u with zeros
     */
    float* u = destination.mBuffer.data();
    
    auto n = destination.mSize;
    
    int i;

    if (source) {
        for(i=0;i<n;i++) u[i] = 0.0;
    }
    else {
        float* fill = source->mBuffer.data();
        for(i=0;i<n;i++) u[i] = fill[i];
    }

    return;
}

void Restrict(Buf& source, Buf& destination)
{
    assert(source.mComponentCount == destination.mComponentCount);
    assert(source.mComponentCount == 1);
    
    float* uc = destination.mBuffer.data();
    float* uf = source.mBuffer.data();
    /* Restrict from fine grid to coarse grid.
     * Uses half weighting.
     * Fine grid has 2^level + 1 points.
     * Coarse grid has 2^(level-1) + 1 points
     */

    int nc,nf;
    int ic, jc, iif, jf;
    int i,j;
    int indx;

    nf = source.mSize;
    nc = destination.mSize;


/* Interior points */
    for(i=1;i<nc-1;i++) {
        for(j=1;j<nc-1;j++) {
            indx = 2*j + 2*i*nf;
#ifdef FULL
            uc[j + nc*i] = .25 * uf[indx]
                            + .125*( uf[indx+1]
                                    + uf[indx -1]
                                    + uf[indx+nf]
                                    + uf[indx-nf])
                            + .0625*( uf[indx+1+nf]
                                    + uf[indx+1-nf]
                                    + uf[indx-1+nf]
                                    + uf[indx-1-nf]);
#else
            uc[j + nc*i] = .5 * uf[indx]
                            + .125*( uf[indx+1]
                                    + uf[indx -1]
                                    + uf[indx+nf]
                                    + uf[indx-nf]);
#endif
        }
    }

/* Boundary Points */

    for(iif = 0; iif < nf; iif += nf-1) {
        for(jf=0; jf < nf; jf+= 2) {
            ic = iif >> 1;
            jc = jf >> 1;
            uc[jc + nc*ic] = uf[jf + iif*nf];
        }
    }

    for(iif=1; iif < nf-1; iif += 2) {
        for(jf=0; jf < nf; jf += nf-1) {
            ic = iif >> 1;
            jc = jf >> 1;
            uc[jc + nc*ic] = uf[jf + nf*iif];
        }
    }
}


void prolongate2D(Buf& source, Buf& destination)
{
    assert(source.mComponentCount == destination.mComponentCount);
    assert(source.mComponentCount == 1);
    
    float* uf = destination.mBuffer.data();
    float* uc = source.mBuffer.data();

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
#ifdef FULL
            uf[indx] = .25*(uf[indx+nf] + uf[indx+1] + uf[indx-nf] + uf[indx-1]);
#else
            uf[indx] = 0.;
#endif
        }
    }
}

void addint(Buf& source, Buf& destination, Buf& destinationFinal) //double *uf, const double *uc, double *res, const int level)
{
    /* Prolongates solution from course mesh to fine mesh
     * and adds that to fine mesh solution
     * level is the course level, where there are 2^level + 1 points
     * res is used for temp storage
     */
    
    float* uf = destinationFinal.mBuffer.data();
    float* res = destination.mBuffer.data();
    int i;
    int n = destinationFinal.mSize;//1 + (2 << (level+1));
    prolongate2D(source, destination);//res,uc,level);
    for(i=0;i<n*n;i++) {
        uf[i] += res[i];
    }

    return;
}


void residual(Buf& source, Buf& residual, Buf& destination) //double *res, const double *u, const double *f, const int level) {
{
    /* Computes the negative residual for the poisson problem. */
    int i,j,indx,n;
    double h,invh2;
    
    float* res = destination.mBuffer.data();
    float* f = residual.mBuffer.data();
    float* u = source.mBuffer.data();

    n = destination.mSize;//(2 << level);

    h = 1./n;
    invh2 = 1./(h*h);
    n+=1;

    for(i=1; i<n-1 ; i++) {
        for(j=1; j <n-1;j++) {
            indx=  j+n*i;

            res[indx] = -invh2*( u[indx+1] + u[indx-1]
                                +u[indx+n] + u[indx-n]
                                - 4. * u[indx]) + f[indx];
        }
    }

    /* Boundary points have zero residual */

    for(i=0;i<n;i++) {
        res[n*i] = 0;
        res[n*i + n-1] = 0;
        res[i] = 0;
        res[i + n*(n-1)] = 0;
    }

    return;
}



void gauss_seidel(Buf& source, Buf& destination, const int numiter)//double *u, const double *f, const int level, const int numiter) {
{
    /* Red-black Gauss-Seidal relaxation, for poisson operator.
     * u contains initial guess
     */
    
float* u = destination.mBuffer.data();
float* f = source.mBuffer.data();


    int i,j,k,indx;
    int n = destination.mSize;//2 << level ;
    double h = 1./n;
    double h2 = h*h;
    //n += 1;

    for(k=0;k<numiter;k++) {

    /* Red Points
     * Exclude boundary values
     */
        for(i=1; i<n-1;i++) {
            for(j=1 + (i&1); j< n-1;j+=2) {    // i&1 = 0 if i is even
                indx = j + n*i;
                u[indx] = .25*(u[indx+1] + u[indx-1]
                            +  u[indx + n] + u[indx-n]
                            - h2*f[indx]);
            }
        }
    /* Black Points */
        for(i=1;i<n-1;i++) {
            for(j=2-(i&1);j<n-1;j+=2) {
                indx= j + n*i;
                u[indx] = .25*(u[indx+1] + u[indx-1]
                            +  u[indx + n] + u[indx-n]
                            - h2*f[indx]);
            }
        }

    }


    return;
}

void CPU::Tick()
{
    Buf advectedVelocity(256, 2);
    Advect(mVelocity, mVelocity, advectedVelocity);
    mVelocity.mBuffer = advectedVelocity.mBuffer;

    Boundary(mVelocity);
    FillVelocity(mVelocity);
    //Buf* divergence = new Buf(256, 1);
    Buf divergence_(256, 1);
    Buf* divergence = &divergence_;
    Divergence(mVelocity, *divergence);

    Buf newPressure(256, 1);
    Jacobi(*divergence, newPressure);

    Buf newVelocity(256, 2);
    Gradient(newPressure, mVelocity, newVelocity);

    mVelocity.mBuffer = newVelocity.mBuffer;

    
    Buf* reduced = new Buf(128, 1);
    Restrict(*divergence, *reduced);
    

/*    if (bgfx::isValid(mTexture))
    {
        bgfx::destroy(mTexture);
    }
  */
    Buf& display = *reduced;
    
    auto mem = bgfx::makeRef(display.mBuffer.data(), display.mBuffer.size() * sizeof(float), ReleaseBufFn, &display);
    bgfx::updateTexture2D(mTexture, 0,0,0,0, display.mSize, display.mSize,mem);
}
