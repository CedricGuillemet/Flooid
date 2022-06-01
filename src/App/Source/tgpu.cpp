#include "tgpu.h"

static const int TEX_SIZE = 256;

Imm::vec3 densityCenter{ 0.5f, 0.1f, 0.f };
Imm::vec3 densityExtend{ 0.05f, 0.05f, 0.f };

TGPU::TGPU()
{

}

void TGPU::DensityGen(TextureProvider& textureProvider)
{
    Imm::matrix m_matrix;
    m_matrix.translation(densityCenter.x, densityCenter.y, densityCenter.z);
    
    Imm::matrix invWorld;
    invWorld.inverse(m_matrix);
    bgfx::setUniform(m_invWorldMatrixUniform, invWorld.m16);
    
    bgfx::setImage(0, m_densityTexture->GetTexture(), 0, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), m_densityGenCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
}

void TGPU::VelocityGen(TextureProvider& textureProvider)
{
    Imm::vec3 m_position = densityCenter;// {0.5f, 0.1f, 0.f};
    float m_radius = densityExtend.x;
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
    bgfx::dispatch(textureProvider.GetViewId(), m_downscaleCSProgram, uint32_t(uf->m_size / 16), uint32_t(uf->m_size / 16));

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
    bgfx::dispatch(textureProvider.GetViewId(), m_upscaleCSProgram, uint32_t(uf->m_size / 16), uint32_t(uf->m_size / 16));
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
    bgfx::dispatch(textureProvider.GetViewId(), m_residualCSProgram, uint32_t(res->m_size / 16), uint32_t(res->m_size / 16));

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
    float hsq = float(level + 1);//sqrtf((level+1)*2);

    if (level == max)
    {
        Jacobi(textureProvider, u, rhs, 50, hsq);
        return;
    }

    int sizeNext = int(fineSize / powf(2.f, hsq));

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
    
    
    /*Texture* */tempRHS = textureProvider.Acquire(PlugType::Any, TEX_SIZE);
    /*Texture* */jacobi[0] = textureProvider.Acquire(PlugType::Any, TEX_SIZE);
    jacobi[1] = textureProvider.Acquire(PlugType::Any, TEX_SIZE);
    tempGradient = textureProvider.Acquire(PlugType::Any, TEX_SIZE);

    ///
    ///
    const int pageSize = 16;
    const int masterSize = 256;
    mWorldToPages = bgfx::createTexture2D(masterSize/pageSize, masterSize/pageSize, false, 0, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_COMPUTE_WRITE);
    mWorldToPageTags = bgfx::createTexture2D(masterSize / pageSize, masterSize / pageSize, false, 0, bgfx::TextureFormat::R8, BGFX_TEXTURE_COMPUTE_WRITE);
    mDensityPages = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mVelocityPages = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mDivergencePages = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mGradientPages = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    
    mJacobiPages[0] = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mJacobiPages[1] = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mDensityAdvectedPages = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mVelocityAdvectedPages = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mResidualPages = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    
    
    mJacobiPagesLevel1[0] = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mJacobiPagesLevel1[1] = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);

    
    mWorldToPagesLevel1 = bgfx::createTexture2D(masterSize / pageSize, masterSize / pageSize, false, 0, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_COMPUTE_WRITE);
    mResidualDownscaledPages = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);

    mAllocatePagesCSProgram = App::LoadProgram("AllocatePages_cs", nullptr);
    mInitPagesCSProgram = App::LoadProgram("InitPages_cs", nullptr);
    mDensityGenPagedCSProgram = App::LoadProgram("DensityGenPaged_cs", nullptr);
    mVelocityGenPagedCSProgram = App::LoadProgram("VelocityGenPaged_cs", nullptr);
    mDilatePagesCSProgram = App::LoadProgram("DilatePages_cs", nullptr);
    mDivergencePagedCSProgram = App::LoadProgram("DivergencePaged_cs", nullptr);
    mJacobiPagedCSProgram = App::LoadProgram("JacobiPaged_cs", nullptr);
    mGradientPagedCSProgram = App::LoadProgram("GradientPaged_cs", nullptr);
    mAdvectPagedCSProgram = App::LoadProgram("AdvectPaged_cs", nullptr);
    mFreePagesCSProgram = App::LoadProgram("FreePages_cs", nullptr);
    mDispatchIndirectCSProgram = App::LoadProgram("DispatchIndirect_cs", nullptr);
    mCommitFreePagesCSProgram = App::LoadProgram("CommitFreePages_cs", nullptr);
    mFrameInitCSProgram = App::LoadProgram("FrameInit_cs", nullptr);
    mResidualPagedCSProgram = App::LoadProgram("ResidualPaged_cs", nullptr);
    mAllocateSubPagesCSProgram = App::LoadProgram("AllocateSubPages_cs", nullptr);
    mDownscalePagedCSProgram = App::LoadProgram("DownscalePaged_cs", nullptr);

    uint32_t pageCount = (256/pageSize) * (256/pageSize);
    
    mBufferCounter = bgfx::createDynamicIndexBuffer(3, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    mBufferPages = bgfx::createDynamicIndexBuffer(pageCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    mBufferAddressPages = bgfx::createDynamicIndexBuffer(pageCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);



    mBufferCounterLevel1 = bgfx::createDynamicIndexBuffer(3, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    mBufferPagesLevel1 = bgfx::createDynamicIndexBuffer(pageCount/4, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    mBufferAddressPagesLevel1 = bgfx::createDynamicIndexBuffer(pageCount/4, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    
    
    mBufferActivePages = bgfx::createDynamicIndexBuffer(pageCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    mBufferFreedPages = bgfx::createDynamicIndexBuffer(pageCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    mBufferActivePageAddresses = bgfx::createDynamicIndexBuffer(pageCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    
    mGroupMinUniform = bgfx::createUniform("groupMin", bgfx::UniformType::Vec4);
    mInitPageCountUniform = bgfx::createUniform("initPageCount", bgfx::UniformType::Vec4);
    mDispatchIndirect = bgfx::createIndirectBuffer(1);
    mDispatchIndirectLevel1 = bgfx::createIndirectBuffer(1);
    
    mTexOutUniform = bgfx::createUniform("s_texOut", bgfx::UniformType::Sampler); //
    mTexWorldToPageUniform = bgfx::createUniform("s_texWorldToPage", bgfx::UniformType::Sampler); //
    mTexPagesUniform = bgfx::createUniform("s_texPages", bgfx::UniformType::Sampler); //

    mDebugDisplayUniform = bgfx::createUniform("debugDisplay", bgfx::UniformType::Vec4); //
    //mFreePages = bgfx::createDynamicIndexBuffer(pageCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
}

void TGPU::ComputeResidual(TextureProvider& textureProvider, bgfx::TextureHandle texU, bgfx::TextureHandle texRHS, bgfx::TextureHandle texWorldToPage, bgfx::TextureHandle texResidual, 
    bgfx::DynamicIndexBufferHandle bufferPages, bgfx::DynamicIndexBufferHandle bufferAddressPages, bgfx::IndirectBufferHandle dispatchIndirect, float hsq)
{
    float invhsq[4] = { 1.f / hsq, 0.f, 0.f, 0.f };
    bgfx::setUniform(m_invhsqUniform, invhsq);

    bgfx::setImage(0, texU, 0, bgfx::Access::Read);
    bgfx::setImage(1, texRHS, 0, bgfx::Access::Read);
    bgfx::setImage(2, texWorldToPage, 0, bgfx::Access::Read);
    bgfx::setBuffer(3, bufferPages, bgfx::Access::Read);
    bgfx::setBuffer(4, bufferAddressPages, bgfx::Access::Read);
    bgfx::setImage(5, texResidual, 0, bgfx::Access::Write);

    bgfx::dispatch(textureProvider.GetViewId(), mResidualPagedCSProgram, dispatchIndirect);
}

void TGPU::Jacobi(TextureProvider& textureProvider, bgfx::TextureHandle texU, bgfx::TextureHandle texRHS, bgfx::TextureHandle texWorldToPage,
    bgfx::DynamicIndexBufferHandle bufferPages, bgfx::DynamicIndexBufferHandle bufferAddressPages, bgfx::IndirectBufferHandle dispatchIndirect, float hsq, int iterationCount)
{
    float jacobiParameters[4] = { hsq, 0.f, 0.f, 0.f };
    bgfx::setUniform(m_jacobiParametersUniform, jacobiParameters);

    bgfx::TextureHandle jacobis[2] = { texU, mJacobiPages[1] };

    bgfx::setImage(0, texU, 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_clearCSProgram, 256 / 16, 256 / 16);

    //int iterationCount = ssteps;
    for (int i = 0; i < iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;

        bgfx::setImage(0, jacobis[indexSource], 0, bgfx::Access::Read);
        bgfx::setImage(1, texWorldToPage, 0, bgfx::Access::Read);
        bgfx::setBuffer(2, bufferAddressPages, bgfx::Access::Read);
        bgfx::setImage(3, texRHS, 0, bgfx::Access::Read);
        bgfx::setBuffer(4, bufferPages, bgfx::Access::Read);
        bgfx::setImage(5, jacobis[indexDestination], 0, bgfx::Access::Write);
        bgfx::dispatch(textureProvider.GetViewId(), mJacobiPagedCSProgram, dispatchIndirect);
    }
}

void TGPU::TestPages(TextureProvider& textureProvider)
{
    // t allocate

    Imm::vec3 wmin = densityCenter;
    wmin -= densityExtend;//{0.1f, 0.1f, 0.0f};
    Imm::vec3 wmax = densityCenter;
    wmax += densityExtend;//{0.4f, 0.4f, 0.0f};*/

    /*Imm::vec3 wmin = { 0.0f, 0.0f, 0.0f };
    Imm::vec3 wmax = { 1.f, 1.f, 0.0f };
    */
    int groupMinx = int(wmin.x * 256.f);
    groupMinx -= groupMinx % 16;

    int groupMiny = int(wmin.y * 256.f);
    groupMiny -= groupMiny % 16;

    int groupMaxx = int(wmax.x * 256.f);
    int modmx = groupMaxx % 16;
    groupMaxx -= modmx;
    groupMaxx += modmx ? 16 : 0;

    int groupMaxy = int(wmax.y * 256.f);
    int modmy = groupMaxy % 16;
    groupMaxy -= modmy;
    groupMaxy += modmy ? 16 : 0;

    groupMinx /= 16;
    groupMiny /= 16;
    groupMaxx /= 16;
    groupMaxy /= 16;

    //float invhsq[4] = { 1.f / hsq, 0.f, 0.f, 0.f };
    //bgfx::setUniform(m_invhsqUniform, invhsq);
    int invocationx = groupMaxx - groupMinx;
    int invocationy = groupMaxy - groupMiny;

    //

    static bool initialized = false;
    // init pages
    if (!initialized)
    {
        initialized = true;

        float pageCount[4] = { 255.f, 0.f, 0.f, 0.f };
        bgfx::setUniform(mInitPageCountUniform, pageCount);
        bgfx::setBuffer(0, mBufferPages, bgfx::Access::Write);
        bgfx::setBuffer(1, mBufferCounter, bgfx::Access::ReadWrite);
        bgfx::dispatch(textureProvider.GetViewId(), mInitPagesCSProgram, 1, 1);

        // allocate pages
        float groupMin[4] = { float(groupMinx), float(groupMiny), 0.f, 0.f };
        bgfx::setUniform(mGroupMinUniform, groupMin);

        //bgfx::setBuffer(0, mFreePages, bgfx::Access::ReadWrite);
        bgfx::setBuffer(0, mBufferAddressPages, bgfx::Access::Write);
        bgfx::setImage(1, mWorldToPages, 0, bgfx::Access::Write);
        bgfx::setBuffer(2, mBufferPages, bgfx::Access::Write);
        bgfx::setBuffer(3, mBufferCounter, bgfx::Access::ReadWrite);
        bgfx::setImage(4, mWorldToPageTags, 0, bgfx::Access::Write);
        bgfx::dispatch(textureProvider.GetViewId(), mAllocatePagesCSProgram, invocationx, invocationy);

        // clear density and velocity

        bgfx::setImage(0, mDensityPages, 0, bgfx::Access::Write);
        bgfx::dispatch(textureProvider.GetViewId(), m_clearCSProgram, 256 / 16, 256 / 16);
        bgfx::setImage(0, mVelocityPages, 0, bgfx::Access::Write);
        bgfx::dispatch(textureProvider.GetViewId(), m_clearCSProgram, 256 / 16, 256 / 16);

    }
    // Frame Init
    bgfx::setBuffer(0, mBufferCounter, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mFrameInitCSProgram, 1, 1);

    // dispatch indirect
    bgfx::setBuffer(0, mDispatchIndirect, bgfx::Access::ReadWrite);
    bgfx::setBuffer(1, mBufferCounter, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mDispatchIndirectCSProgram, 1, 1);
    
    // density
    float position[4] = { densityCenter.x, densityCenter.y, densityCenter.z, densityExtend.x };
    bgfx::setUniform(m_positionUniform, position);

    bgfx::setBuffer(1, mBufferAddressPages, bgfx::Access::Read);
    bgfx::setBuffer(2, mBufferPages, bgfx::Access::Read);
    bgfx::setImage(0, mDensityPages, 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), mDensityGenPagedCSProgram, mDispatchIndirect);

    // velocity
    float direction[4] = { 0.f, 1.f, 0.f, 0.f };
    bgfx::setUniform(m_directionUniform, direction);

    bgfx::setBuffer(1, mBufferAddressPages, bgfx::Access::Read);
    bgfx::setBuffer(2, mBufferPages, bgfx::Access::Read);
    bgfx::setImage(0, mVelocityPages, 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), mVelocityGenPagedCSProgram, mDispatchIndirect);
    
    // advect density
    bgfx::setImage(0, mVelocityPages, 0, bgfx::Access::Read);
    bgfx::setImage(1, mDensityPages, 0, bgfx::Access::Read);
    bgfx::setImage(2, mWorldToPages, 0, bgfx::Access::Read);
    bgfx::setBuffer(3, mBufferAddressPages, bgfx::Access::Read);
    bgfx::setImage(4, mDensityAdvectedPages, 0, bgfx::Access::Write);
    bgfx::setBuffer(5, mBufferPages, bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mAdvectPagedCSProgram, mDispatchIndirect);

    // advect velocity
    bgfx::setImage(0, mVelocityPages, 0, bgfx::Access::Read);
    bgfx::setImage(1, mVelocityPages, 0, bgfx::Access::Read);
    bgfx::setImage(2, mWorldToPages, 0, bgfx::Access::Read);
    bgfx::setBuffer(3, mBufferAddressPages, bgfx::Access::Read);
    bgfx::setImage(4, mVelocityAdvectedPages, 0, bgfx::Access::Write);
    bgfx::setBuffer(5, mBufferPages, bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mAdvectPagedCSProgram, mDispatchIndirect);
    
    // free pages
    bgfx::setBuffer(0, mBufferAddressPages, bgfx::Access::Read);
    bgfx::setBuffer(1, mBufferPages, bgfx::Access::Read);
    bgfx::setImage(2, mWorldToPageTags, 0, bgfx::Access::Write);
    bgfx::setImage(3, mDensityAdvectedPages, 0, bgfx::Access::Read);
    bgfx::setBuffer(4, mBufferCounter, bgfx::Access::ReadWrite);
    bgfx::setBuffer(5, mBufferActivePages, bgfx::Access::ReadWrite);
    bgfx::setBuffer(6, mBufferFreedPages, bgfx::Access::ReadWrite);
    bgfx::setBuffer(7, mBufferActivePageAddresses, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mFreePagesCSProgram, mDispatchIndirect);
    
    // commit pages
    bgfx::setBuffer(0, mBufferPages, bgfx::Access::ReadWrite);
    bgfx::setBuffer(1, mBufferCounter, bgfx::Access::ReadWrite);
    bgfx::setBuffer(2, mBufferAddressPages, bgfx::Access::ReadWrite);
    bgfx::setBuffer(3, mBufferActivePages, bgfx::Access::ReadWrite);
    bgfx::setBuffer(4, mBufferFreedPages, bgfx::Access::ReadWrite);
    bgfx::setBuffer(5, mBufferActivePageAddresses, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mCommitFreePagesCSProgram, 1, 1);
    
    // Dilate pages
    bgfx::setBuffer(0, mBufferAddressPages, bgfx::Access::Write);
    bgfx::setImage(1, mWorldToPages, 0, bgfx::Access::Write);
    bgfx::setBuffer(2, mBufferPages, bgfx::Access::Write);
    bgfx::setBuffer(3, mBufferCounter, bgfx::Access::ReadWrite);
    bgfx::setImage(4, mWorldToPageTags, 0, bgfx::Access::Write);
    bgfx::setImage(5, mVelocityAdvectedPages, 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), mDilatePagesCSProgram, (256 / 16) / 16, (256 / 16) / 16); 
    
    // dispatch indirect
    bgfx::setBuffer(0, mDispatchIndirect, bgfx::Access::ReadWrite);
    bgfx::setBuffer(1, mBufferCounter, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mDispatchIndirectCSProgram, 1, 1);
    
    // Divergence
    bgfx::setBuffer(2, mBufferAddressPages, bgfx::Access::Read);
    bgfx::setImage(3, mDivergencePages, 0, bgfx::Access::Write);
    bgfx::setImage(1, mWorldToPages, 0, bgfx::Access::Read);
    bgfx::setImage(0, mVelocityAdvectedPages, 0, bgfx::Access::Read);
    bgfx::setBuffer(4, mBufferPages, bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mDivergencePagedCSProgram, mDispatchIndirect);
  
    // Jacobi
    int level = 0;

    int ssteps = 4;
    float hsq = float(level + 1);
    bgfx::TextureHandle rhs = mDivergencePages;

    Jacobi(textureProvider, mJacobiPages[0], rhs, mWorldToPages, mBufferPages, mBufferAddressPages, mDispatchIndirect, hsq, 50);

    // Residual
    ComputeResidual(textureProvider, mJacobiPages[0], rhs, mWorldToPages, mResidualPages, mBufferPages, mBufferAddressPages, mDispatchIndirect, hsq);

    // next level -------------------------------

    // init
    float pageCount[4] = { 63.f, 0.f, 0.f, 0.f };
    bgfx::setUniform(mInitPageCountUniform, pageCount);
    bgfx::setBuffer(0, mBufferPagesLevel1, bgfx::Access::Write);
    bgfx::setBuffer(1, mBufferCounterLevel1, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mInitPagesCSProgram, 1, 1);

    // new indirection map
    bgfx::setBuffer(0, mBufferAddressPagesLevel1, bgfx::Access::Write);
    bgfx::setImage(1, mWorldToPagesLevel1, 0, bgfx::Access::Write);
    bgfx::setBuffer(2, mBufferPagesLevel1, bgfx::Access::ReadWrite);
    bgfx::setBuffer(3, mBufferCounterLevel1, bgfx::Access::ReadWrite);
    bgfx::setImage(4, mWorldToPageTags, 0, bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mAllocateSubPagesCSProgram, 1, 1);

    // dispatch indirect
    bgfx::setBuffer(0, mDispatchIndirectLevel1, bgfx::Access::ReadWrite);
    bgfx::setBuffer(1, mBufferCounterLevel1, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mDispatchIndirectCSProgram, 1, 1);

    // downscale filter
    bgfx::setImage(0, mResidualPages, 0, bgfx::Access::Read);
    bgfx::setImage(1, mWorldToPages, 0, bgfx::Access::Read);
    bgfx::setImage(2, mResidualDownscaledPages, 0, bgfx::Access::Write);
    bgfx::setBuffer(3, mBufferAddressPagesLevel1, bgfx::Access::Read);
    bgfx::setBuffer(4, mBufferPagesLevel1, bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mDownscalePagedCSProgram, mDispatchIndirectLevel1);
    
    // jacobi level 1
    level = 1;
    hsq = float(level + 1);
    Jacobi(textureProvider, mJacobiPagesLevel1[0], mResidualDownscaledPages, mWorldToPagesLevel1, mBufferPagesLevel1, mBufferAddressPagesLevel1, mDispatchIndirectLevel1, hsq, 50);
    // -------------------------------------------
    // 
    // gradient
    bgfx::setImage(0, mJacobiPages[0], 0, bgfx::Access::Read);
    bgfx::setImage(1, mVelocityAdvectedPages, 0, bgfx::Access::Read);
    bgfx::setImage(2, mWorldToPages, 0, bgfx::Access::Read);
    bgfx::setBuffer(3, mBufferAddressPages, bgfx::Access::Read);
    bgfx::setImage(4, mGradientPages, 0, bgfx::Access::Write);
    bgfx::setBuffer(5, mBufferPages, bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mGradientPagedCSProgram, mDispatchIndirect);

    std::swap(mGradientPages, mVelocityPages);
    std::swap(mDensityPages, mDensityAdvectedPages);
}

void TGPU::TestVCycle(TextureProvider& textureProvider)
{
    VelocityGen(textureProvider);
    DensityGen(textureProvider);

    // advection

    Texture* advectedVelocity = textureProvider.Acquire(PlugType::Any, 256);
    Texture* newDensity = textureProvider.Acquire(PlugType::Any, TEX_SIZE);
    Advect(textureProvider, m_densityTexture, m_velocityTexture, newDensity);
    textureProvider.Release(m_densityTexture);

    Advect(textureProvider, m_velocityTexture, m_velocityTexture, advectedVelocity);


    //Texture* tempRHS = textureProvider.Acquire(PlugType::Any, TEX_SIZE);
    Divergence(textureProvider, advectedVelocity, tempRHS);


    //Texture* jacobi[2] = { textureProvider.Acquire(PlugType::Any, TEX_SIZE), textureProvider.Acquire(PlugType::Any, TEX_SIZE) };
    bgfx::setImage(0, jacobi[0]->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_clearCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    //Jacobi(textureProvider, jacobi, tempRHS, 100);
    vcycle(textureProvider, tempRHS, jacobi[0], 256, 0, 0);

    //textureProvider.Release(tempRHS);
    //textureProvider.Release(jacobi[1]);

    Texture* newVelocity = textureProvider.Acquire(PlugType::Any, TEX_SIZE);
    Gradient(textureProvider, jacobi[0], advectedVelocity, newVelocity);
    Gradient(textureProvider, jacobi[0], advectedVelocity, tempGradient);

    //textureProvider.Release(jacobi[0]);

    textureProvider.Release(advectedVelocity);

    textureProvider.Release(m_velocityTexture);

    m_densityTexture = newDensity;
    m_velocityTexture = newVelocity;
}

void TGPU::Tick(TextureProvider& textureProvider)
{
    // debug display
    float debugDisplay[4] = { mDebugGrid ? 1.f : 0.f, mDebugPageAllocation ? 1.f : 0.f, float(mDebugDisplay), 0.f };
    bgfx::setUniform(mDebugDisplayUniform, debugDisplay);
    bgfx::touch(textureProvider.GetViewId());

    if (mCurrentFrame < mDesiredFrame)
    {
        mCurrentFrame++;
        TestPages(textureProvider);

        TestVCycle(textureProvider);
    }
}


void TGPU::UI()
{
    if (ImGui::Button("Next"))
    {
        mDesiredFrame++;
    }
    ImGui::SameLine();
    if (ImGui::Button("Play"))
    {
        mDesiredFrame+=9999999;
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop"))
    {
        mCurrentFrame = mDesiredFrame;
    }
    //ImGui::Combo("Display", &mDebugDisplay, "Density\0Velocity\0Page Tag\0Divergence\0Jacobi\0Gradient\0Residual Mip0\0VCycle Density\0VCycle Velocity\0VCycle Divergence\0VCycle Jacobi\0");
    if (ImGui::RadioButton("Density", mDebugDisplay == 0)) mDebugDisplay = 0;
    if (ImGui::RadioButton("Velocity", mDebugDisplay == 1)) mDebugDisplay = 1;
    if (ImGui::RadioButton("Page Tag", mDebugDisplay == 2)) mDebugDisplay = 2;
    if (ImGui::RadioButton("Divergence", mDebugDisplay == 3)) mDebugDisplay = 3;
    if (ImGui::RadioButton("Jacobi", mDebugDisplay == 4)) mDebugDisplay = 4;
    if (ImGui::RadioButton("Gradient", mDebugDisplay == 5)) mDebugDisplay = 5;
    if (ImGui::RadioButton("Residual Mip0", mDebugDisplay == 6)) mDebugDisplay = 6;
    if (ImGui::RadioButton("Residual Mip1", mDebugDisplay == 7)) mDebugDisplay = 7;
    if (ImGui::RadioButton("Jacobi Mip1", mDebugDisplay == 8)) mDebugDisplay = 8;
    /*if (ImGui::RadioButton("VCycle Density", mDebugDisplay == 8)) mDebugDisplay = 8;
    if (ImGui::RadioButton("VCycle Velocity", mDebugDisplay == 9)) mDebugDisplay = 9;
    if (ImGui::RadioButton("VCycle Divergence", mDebugDisplay == 10)) mDebugDisplay = 10;
    if (ImGui::RadioButton("VCycle Jacobi", mDebugDisplay == 11)) mDebugDisplay = 11;
    if (ImGui::RadioButton("VCycle Gradient", mDebugDisplay == 12)) mDebugDisplay = 12;
    */
    ImGui::Checkbox("Grid", &mDebugGrid);
    ImGui::Checkbox("Page Allocation", &mDebugPageAllocation);
}

bgfx::TextureHandle TGPU::GetDisplayPages() const
{
    switch (mDebugDisplay)
    {
    case 0: return mDensityPages;
    case 1: return mVelocityPages;
    case 2: return mDensityPages;
    case 3: return mDivergencePages;
    case 4: return mJacobiPages[0];
    case 5: return mGradientPages;
    case 6: return mResidualPages;
    case 7: return mResidualDownscaledPages;
    case 8: return mJacobiPagesLevel1[0];
            
/*
    case 8: return m_densityTexture->GetTexture();
    case 9: return m_velocityTexture->GetTexture();
    case 10: return tempRHS->GetTexture();
    case 11: return jacobi[0]->GetTexture();
    case 12: return tempGradient->GetTexture();
  */

    }
    return { bgfx::kInvalidHandle };
}

bgfx::TextureHandle TGPU::GetDisplayPageIndirection() const
{
    switch (mDebugDisplay)
    {
        case 8:
        case 7: return mWorldToPagesLevel1;
        default: return mWorldToPages;
    }
    return { bgfx::kInvalidHandle };
}
