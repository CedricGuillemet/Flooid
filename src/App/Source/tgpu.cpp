#include "tgpu.h"

static const int TEX_SIZE = 256;
TGPU::TGPU()
{

}

void TGPU::DensityGen(TextureProvider& textureProvider)
{
    Imm::matrix m_matrix;
    m_matrix.translation(0.5f, 0.1f, 0.f);
    
    Imm::matrix invWorld;
    invWorld.inverse(m_matrix);
    bgfx::setUniform(m_invWorldMatrixUniform, invWorld.m16);
    
    bgfx::setImage(0, m_densityTexture->GetTexture(), 0, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), m_densityGenCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
}

void TGPU::VelocityGen(TextureProvider& textureProvider)
{
    Imm::vec3 m_position{0.5f, 0.1f, 0.f};
    float m_radius = 0.07;
    float position[4] = { m_position.x, m_position.y, m_position.z, m_radius };
    bgfx::setUniform(m_positionUniform, position);

    Imm::matrix matrix;
    Imm::vec3 scale{m_radius, m_radius, m_radius};
    //ImGuizmo::RecomposeMatrixFromComponents(&m_position.x, &m_orientation.x, &scale.x, matrix.m16);
    float direction[4] = { 0.f, 0.1f, 0.f, 0.f};//matrix.dir.x, matrix.dir.y, matrix.dir.z, 0.f };
    bgfx::setUniform(m_directionUniform, direction);

    bgfx::setImage(0, m_velocityTexture->GetTexture(), 0, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), m_velocityGenCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
}

void TGPU::Advect(TextureProvider& textureProvider, Texture* source, Texture* velocity, Texture* output)
{
    float advection[4] = { 1.f/*m_timeScale*/, 0.998f/*m_dissipation*/, 0.f, 0.f };
    bgfx::setUniform(m_advectionUniform, advection);

    bgfx::setTexture(0, m_texVelocityUniform, velocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setTexture(1, m_texAdvectUniform, source->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setImage(2, output->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_advectCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
}

void TGPU::Gradient(TextureProvider& textureProvider, Texture* u, Texture* velocity, Texture* destination)
{
    bgfx::setTexture(0, m_texPressureUniform, u->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setTexture(1, m_texVelocityUniform, velocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(2, destination->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_gradientCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
}

void TGPU::Divergence(TextureProvider& textureProvider, Texture* velocity, Texture* destination)
{
    bgfx::setTexture(0, m_texVelocityUniform, velocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(1, destination->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_divergenceCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
}

void TGPU::Jacobi(TextureProvider& textureProvider, Texture* jacobi[2], const Texture* rhs, int iterationCount)
{
    float hsq = 1.f;
    float jacobiParameters[4] = { hsq, 0.f, 0.f, 0.f };
    bgfx::setUniform(m_jacobiParametersUniform, jacobiParameters);

    for (int i = 0; i < iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;

        bgfx::setTexture(0, m_texUUniform, jacobi[indexSource]->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setTexture(1, m_texRHSUniform, rhs->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setImage(2, jacobi[indexDestination]->GetTexture(), 0, bgfx::Access::Write);
        bgfx::dispatch(textureProvider.GetViewId(), m_jacobiCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    }
}


void TGPU::Jacobi(TextureProvider& textureProvider, Texture* u, const Texture* rhs, int iterationCount, float hsq)
{
    float jacobiParameters[4] = { hsq, 0.f, 0.f, 0.f };
    bgfx::setUniform(m_jacobiParametersUniform, jacobiParameters);

    Texture* jacobiBuf = textureProvider.Acquire(PlugType::Any, u->m_size);

    Texture* jacobis[2] = { u, jacobiBuf };

    for (int i = 0; i < iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;

        bgfx::setTexture(0, m_texUUniform, jacobis[indexSource]->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setTexture(1, m_texRHSUniform, rhs->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setImage(2, jacobis[indexDestination]->GetTexture(), 0, bgfx::Access::Write);
        bgfx::dispatch(textureProvider.GetViewId(), m_jacobiCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    }
    textureProvider.Release(jacobiBuf);
}


void TGPU::coarsen(TextureProvider& textureProvider, const Texture* uf, Texture* uc)
{
    /*assert(uf.mSize == uc.mSize * 2);
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
    }*/
    bgfx::setTexture(0, m_texUUniform, uf->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(1, uc->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_downscaleCSProgram, uf->m_size / 16, uf->m_size / 16);

}

void TGPU::refine_and_add(TextureProvider& textureProvider, const Texture* u, Texture* uf)
{
    /*for (int jc = 1; jc < u.mSize; jc++)
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
    }*/

    float refineParameters[4] = { static_cast<float>(uf->m_size), 0.f, 0.f, 0.f };
    bgfx::setUniform(m_fineTexSizeUniform, refineParameters);


    bgfx::setTexture(0, m_texUUniform, u->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setImage(1, uf->GetTexture(), 0, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), m_upscaleCSProgram, uf->m_size / 16, uf->m_size / 16);
}

void TGPU::compute_residual(TextureProvider& textureProvider, const Texture* u, const Texture* rhs, Texture* res, float hsq)
{
    /*const float invhsq = 1.f / hsq;
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
    }*/

    float invhsq[4] = { 1.f / hsq, 0.f, 0.f, 0.f };
    bgfx::setUniform(m_invhsqUniform, invhsq);


    bgfx::setTexture(0, m_texUUniform, u->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setTexture(1, m_texRHSUniform, rhs->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(2, res->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_residualCSProgram, res->m_size / 16, res->m_size / 16);

}

void TGPU::compute_and_coarsen_residual(TextureProvider& textureProvider, const Texture* u, const Texture* rhs, Texture* resc, float hsq)
{
    //Buf resf(u.mSize, 1);
    Texture* resf = textureProvider.Acquire(PlugType::Any, u->m_size);
    //resf.Set(0.f);
    compute_residual(textureProvider, u, rhs, resf, hsq);
    coarsen(textureProvider, resf, resc);
    textureProvider.Release(resf);
}

void TGPU::vcycle(TextureProvider& textureProvider, const Texture* rhs, Texture* u, int fineSize, int level, int max)
{
    int ssteps = 4;
    float hsq = level + 1;//sqrtf((level+1)*2);

    if (level == max)
    {
        Jacobi(textureProvider, u, rhs, 50, hsq);
        return;
    }

    int sizeNext = fineSize / powf(2.f, level + 1);

    //Buf rhsNext(sizeNext, 1);
    //Buf uNext(sizeNext, 1);
    Texture* rhsNext = textureProvider.Acquire(PlugType::Any, sizeNext);
    Texture* uNext = textureProvider.Acquire(PlugType::Any, sizeNext);

    Jacobi(textureProvider, u, rhs, ssteps, hsq);
    compute_and_coarsen_residual(textureProvider, u, rhs, rhsNext, hsq);
    //uNext.Set(0.f);
    bgfx::setImage(0, uNext->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_clearCSProgram, sizeNext / 16, sizeNext / 16);


    vcycle(textureProvider, rhsNext, uNext, fineSize, level + 1, max);
    textureProvider.Release(rhsNext);
    refine_and_add(textureProvider, uNext, u);
    textureProvider.Release(uNext);
    Jacobi(textureProvider, u, rhs, ssteps, hsq);
}

void TGPU::Init(TextureProvider& textureProvider)
{
    m_densityTexture = textureProvider.Acquire(PlugType::Any, 256);
    m_velocityTexture = textureProvider.Acquire(PlugType::Any, 256);
    
    m_densityGenCSProgram = App::LoadProgram("DensityGen_cs", nullptr);
    m_invWorldMatrixUniform = bgfx::createUniform("invWorldMatrix", bgfx::UniformType::Mat4);


    m_velocityGenCSProgram = App::LoadProgram("VelocityGen_cs", nullptr);
    m_positionUniform = bgfx::createUniform("position", bgfx::UniformType::Vec4);
    m_directionUniform = bgfx::createUniform("direction", bgfx::UniformType::Vec4);


    m_jacobiCSProgram = App::LoadProgram("Jacobi_cs", nullptr);
    m_divergenceCSProgram = App::LoadProgram("Divergence_cs", nullptr);
    m_gradientCSProgram = App::LoadProgram("Gradient_cs", nullptr);
    m_clearCSProgram = App::LoadProgram("Clear_cs", nullptr);
    m_jacobiParametersUniform = bgfx::createUniform("jacobiParameters", bgfx::UniformType::Vec4);
    m_texVelocityUniform = bgfx::createUniform("s_texVelocity", bgfx::UniformType::Sampler);
    m_texJacoviUniform = bgfx::createUniform("s_texJacobi", bgfx::UniformType::Sampler);
    m_texDivergenceUniform = bgfx::createUniform("s_texDivergence", bgfx::UniformType::Sampler);
    m_texPressureUniform = bgfx::createUniform("s_texPressure", bgfx::UniformType::Sampler);

    m_advectCSProgram = App::LoadProgram("Advect_cs", nullptr);
    m_advectionUniform = bgfx::createUniform("advection", bgfx::UniformType::Vec4);
    m_texAdvectUniform = bgfx::createUniform("s_texAdvect", bgfx::UniformType::Sampler);


    m_downscaleCSProgram = App::LoadProgram("Downscale_cs", nullptr);
    m_upscaleCSProgram = App::LoadProgram("Upscale_cs", nullptr);
    m_residualCSProgram = App::LoadProgram("Residual_cs", nullptr);

    m_texUUniform = bgfx::createUniform("s_texU", bgfx::UniformType::Sampler);
    m_texRHSUniform = bgfx::createUniform("s_texRHS", bgfx::UniformType::Sampler);

    m_invhsqUniform = bgfx::createUniform("invhsq", bgfx::UniformType::Vec4);
    m_fineTexSizeUniform = bgfx::createUniform("fineTexSize", bgfx::UniformType::Vec4);
    
    ///
    ///
    const int pageSize = 16;
    const int masterSize = 256;
    mWorldToPages = bgfx::createTexture2D(masterSize/pageSize, masterSize/pageSize, false, 0, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_COMPUTE_WRITE);
    mDensityPages = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    
    mAllocatePagesCSProgram = App::LoadProgram("AllocatePages_cs", nullptr);
    mInitPagesCSProgram = App::LoadProgram("InitPages_cs", nullptr);
    mDensityGenPagedCSProgram = App::LoadProgram("DensityGenPaged_cs", nullptr);
    
    uint32_t pageCount = (256/pageSize) * (256/pageSize);
    
    mBufferCounter = bgfx::createDynamicIndexBuffer(3, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    mBufferPages = bgfx::createDynamicIndexBuffer(pageCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    mBufferAddressPages = bgfx::createDynamicIndexBuffer(pageCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    mGroupMinUniform = bgfx::createUniform("groupMin", bgfx::UniformType::Vec4);
    mTexOutUniform = bgfx::createUniform("s_texOut", bgfx::UniformType::Sampler); //
    

    mFreePages = bgfx::createDynamicIndexBuffer(pageCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
}

void TGPU::TestPages(TextureProvider& textureProvider)
{
    // t allocate
    Imm::vec3 densityCenter{0.25f, 0.25f, 0.f};
    Imm::vec3 densityExtend{0.15f, 0.15f, 0.f};
    Imm::vec3 wmin = {0.1f, 0.1f, 0.0f};
    Imm::vec3 wmax = {0.4f, 0.4f, 0.0f};
    
    int groupMinx = wmin.x * 256;
    groupMinx -= groupMinx % 16;

    int groupMiny = wmin.y * 256;
    groupMiny -= groupMiny % 16;

    int groupMaxx = wmax.x * 256;
    int modmx = groupMaxx % 16;
    groupMaxx -= modmx;
    groupMaxx += modmx ? 16 : 0;

    int groupMaxy = wmax.y * 256;
    int modmy = groupMaxy % 16;
    groupMaxy -= modmy;
    groupMaxy += modmy ? 16 : 0;

    groupMinx /= 16;
    groupMiny /= 16;
    groupMaxx /= 16;
    groupMaxy /= 16;

    //float invhsq[4] = { 1.f / hsq, 0.f, 0.f, 0.f };
    //bgfx::setUniform(m_invhsqUniform, invhsq);
    int invocationx = groupMaxx - groupMinx + 1;
    int invocationy = groupMaxy - groupMiny + 1;

    
    // init pages
    
    
    bgfx::setBuffer(0, mFreePages, bgfx::Access::Write);
    bgfx::setBuffer(1, mBufferCounter, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mInitPagesCSProgram, 1, 1);
    
    
    // allocate pages
    float groupMin[4] = { float(groupMinx), float(groupMiny), 0.f, 0.f };
    bgfx::setUniform(mGroupMinUniform, groupMin);

    
    //bgfx::setTexture(0, m_texUUniform, u->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    //bgfx::setTexture(1, m_texRHSUniform, rhs->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setBuffer(0, mFreePages, bgfx::Access::Read);
    bgfx::setBuffer(1, mBufferAddressPages, bgfx::Access::Write);
    bgfx::setBuffer(3, mBufferPages, bgfx::Access::Write);
    bgfx::setBuffer(4, mBufferCounter, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mAllocatePagesCSProgram, invocationx, invocationy);

    
    bgfx::setBuffer(1, mBufferAddressPages, bgfx::Access::Write);
    bgfx::setBuffer(3, mBufferPages, bgfx::Access::Write);


    float position[4] = { densityCenter.x, densityCenter.y, densityCenter.z, densityExtend.x };
    bgfx::setUniform(m_positionUniform, position);

    bgfx::setImage(0, mDensityPages, 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), mDensityGenPagedCSProgram, 1, invocationx * invocationy);
    
}

void TGPU::Tick(TextureProvider& textureProvider)
{
    /*VelocityGen(textureProvider);
    DensityGen(textureProvider);

    // advection

    Texture* advectedVelocity = textureProvider.Acquire(PlugType::Any, 256);
    Texture* newDensity = textureProvider.Acquire(PlugType::Any, TEX_SIZE);
    Advect(textureProvider, m_densityTexture, m_velocityTexture, newDensity);
    textureProvider.Release(m_densityTexture);
    
    Advect(textureProvider, m_velocityTexture, m_velocityTexture, advectedVelocity);
    
    
    Texture* tempRHS = textureProvider.Acquire(PlugType::Any, TEX_SIZE);
    Divergence(textureProvider, advectedVelocity, tempRHS);
    
    
    Texture* jacobi[2] = {textureProvider.Acquire(PlugType::Any, TEX_SIZE), textureProvider.Acquire(PlugType::Any, TEX_SIZE)};
    bgfx::setImage(0, jacobi[0]->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_clearCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    //Jacobi(textureProvider, jacobi, tempRHS, 100);
    vcycle(textureProvider, tempRHS, jacobi[0], 256, 0, 4);
    
    textureProvider.Release(tempRHS);
    textureProvider.Release(jacobi[1]);
    
    Texture* newVelocity = textureProvider.Acquire(PlugType::Any, TEX_SIZE);
    Gradient(textureProvider, jacobi[0], advectedVelocity, newVelocity);
    
    textureProvider.Release(jacobi[0]);
    
    textureProvider.Release(advectedVelocity);
    
    textureProvider.Release(m_velocityTexture);
    
    m_densityTexture = newDensity;
    m_velocityTexture = newVelocity;
     */
    TestPages(textureProvider);
}
