#include "tgpu.h"

static const int TEX_SIZE = 256;

Imm::vec3 densityCenter{ 0.5f, 0.1f, 0.f };
Imm::vec3 densityExtend{ 0.05f, 0.05f, 0.f };

TGPU::TGPU()
{

}

void TGPU::Init(TextureProvider& textureProvider)
{
    const int pageSize = 16;
    const int masterSize = 256;
    mWorldToPageTags = bgfx::createTexture2D(masterSize / pageSize, masterSize / pageSize, false, 0, bgfx::TextureFormat::R8, BGFX_TEXTURE_COMPUTE_WRITE);
    mDensityPages = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mVelocityPages = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mDivergencePages = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mGradientPages = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    
    mTempPages = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mDensityAdvectedPages = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
    mVelocityAdvectedPages = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);

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
    mUpscalePagedCSProgram = App::LoadProgram("UpscalePaged_cs", nullptr);
    mClearCSProgram = App::LoadProgram("Clear_cs", nullptr);
    mClearPagesCSProgram = App::LoadProgram("ClearPages_cs", nullptr);
    
    m_jacobiParametersUniform = bgfx::createUniform("jacobiParameters", bgfx::UniformType::Vec4);
    m_invhsqUniform = bgfx::createUniform("invhsq", bgfx::UniformType::Vec4);
    m_positionUniform = bgfx::createUniform("position", bgfx::UniformType::Vec4);
    m_directionUniform = bgfx::createUniform("direction", bgfx::UniformType::Vec4);

    uint32_t pageCount = (256/pageSize) * (256/pageSize);
        
    mBufferActivePages = bgfx::createDynamicIndexBuffer(pageCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    mBufferFreedPages = bgfx::createDynamicIndexBuffer(pageCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    mBufferActivePageAddresses = bgfx::createDynamicIndexBuffer(pageCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
    
    mGroupMinUniform = bgfx::createUniform("groupMin", bgfx::UniformType::Vec4);
    mInitPageCountUniform = bgfx::createUniform("initPageCount", bgfx::UniformType::Vec4);
    
    for (int i = 0; i < MaxLevel; i++)
    {
        mDispatchIndirect[i] = bgfx::createIndirectBuffer(1);
        
        mBufferCounter[i] = bgfx::createDynamicIndexBuffer(3, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
        mBufferPages[i] = bgfx::createDynamicIndexBuffer(pageCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);
        mBufferAddressPages[i] = bgfx::createDynamicIndexBuffer(pageCount, BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ_WRITE);

        mWorldToPages[i] = bgfx::createTexture2D(masterSize/pageSize, masterSize/pageSize, false, 0, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_COMPUTE_WRITE);
        mJacobiPages[i] = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);
        mResidualPages[i] = bgfx::createTexture2D(masterSize, masterSize, false, 0, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_COMPUTE_WRITE);

    }
    
    mTexWorldToPageUniform = bgfx::createUniform("s_texWorldToPage", bgfx::UniformType::Sampler); //

    mDebugDisplayUniform = bgfx::createUniform("debugDisplay", bgfx::UniformType::Vec4); //
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

    bgfx::TextureHandle jacobis[2] = { texU, mTempPages };

    for (int i = 0; i < iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;

        bgfx::setImage(0, jacobis[indexSource], 0, bgfx::Access::Read);
        bgfx::setImage(1, texRHS, 0, bgfx::Access::Read);
        bgfx::setImage(2, texWorldToPage, 0, bgfx::Access::Read);
        bgfx::setBuffer(3, bufferAddressPages, bgfx::Access::Read);
        bgfx::setBuffer(4, bufferPages, bgfx::Access::Read);
        bgfx::setImage(5, jacobis[indexDestination], 0, bgfx::Access::Write);
        bgfx::dispatch(textureProvider.GetViewId(), mJacobiPagedCSProgram, dispatchIndirect);
    }
}

void TGPU::ClearTexture(TextureProvider& textureProvider, bgfx::TextureHandle texture)
{
    bgfx::setImage(0, texture, 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), mClearCSProgram, 256 / 16, 256 / 16);
}

void TGPU::ClearPages(TextureProvider& textureProvider, bgfx::TextureHandle pages, bgfx::DynamicIndexBufferHandle bufferPages, bgfx::IndirectBufferHandle dispatchIndirect)
{
    bgfx::setImage(0, pages, 0, bgfx::Access::Write);
    bgfx::setBuffer(1, bufferPages, bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mClearPagesCSProgram, dispatchIndirect);
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
        for (int i = 0; i < MaxLevel; i++)
        {
            ClearTexture(textureProvider, mWorldToPages[i]);
        }
        
        ClearTexture(textureProvider, mWorldToPageTags);
        initialized = true;

        float pageCount[4] = { 255.f, 0.f, 0.f, 0.f };
        bgfx::setUniform(mInitPageCountUniform, pageCount);
        bgfx::setBuffer(0, mBufferPages[0], bgfx::Access::Write);
        bgfx::setBuffer(1, mBufferCounter[0], bgfx::Access::ReadWrite);
        bgfx::dispatch(textureProvider.GetViewId(), mInitPagesCSProgram, 1, 1);

        // allocate pages
        float groupMin[4] = { float(groupMinx), float(groupMiny), 0.f, 0.f };
        bgfx::setUniform(mGroupMinUniform, groupMin);

        //bgfx::setBuffer(0, mFreePages, bgfx::Access::ReadWrite);
        bgfx::setBuffer(0, mBufferAddressPages[0], bgfx::Access::Write);
        bgfx::setImage(1, mWorldToPages[0], 0, bgfx::Access::Write);
        bgfx::setBuffer(2, mBufferPages[0], bgfx::Access::Write);
        bgfx::setBuffer(3, mBufferCounter[0], bgfx::Access::ReadWrite);
        bgfx::setImage(4, mWorldToPageTags, 0, bgfx::Access::Write);
        bgfx::dispatch(textureProvider.GetViewId(), mAllocatePagesCSProgram, invocationx, invocationy);

        // clear density and velocity
        ClearTexture(textureProvider, mDensityPages);
        ClearTexture(textureProvider, mVelocityPages);
    }
    // Frame Init
    bgfx::setBuffer(0, mBufferCounter[0], bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mFrameInitCSProgram, 1, 1);

    // dispatch indirect
    bgfx::setBuffer(0, mDispatchIndirect[0], bgfx::Access::ReadWrite);
    bgfx::setBuffer(1, mBufferCounter[0], bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mDispatchIndirectCSProgram, 1, 1);
    
    // density
    float position[4] = { densityCenter.x, densityCenter.y, densityCenter.z, densityExtend.x };
    bgfx::setUniform(m_positionUniform, position);

    bgfx::setImage(0, mDensityPages, 0, bgfx::Access::Write);
    bgfx::setBuffer(1, mBufferAddressPages[0], bgfx::Access::Read);
    bgfx::setBuffer(2, mBufferPages[0], bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mDensityGenPagedCSProgram, mDispatchIndirect[0]);

    // velocity
    float direction[4] = { 0.f, 1.f, 0.f, 0.f };
    bgfx::setUniform(m_directionUniform, direction);

    bgfx::setImage(0, mVelocityPages, 0, bgfx::Access::Write);
    bgfx::setBuffer(1, mBufferAddressPages[0], bgfx::Access::Read);
    bgfx::setBuffer(2, mBufferPages[0], bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mVelocityGenPagedCSProgram, mDispatchIndirect[0]);
    
    // advect density
    bgfx::setImage(0, mDensityPages, 0, bgfx::Access::Read);
    bgfx::setImage(1, mVelocityPages, 0, bgfx::Access::Read);
    bgfx::setImage(2, mWorldToPages[0], 0, bgfx::Access::Read);
    bgfx::setBuffer(3, mBufferAddressPages[0], bgfx::Access::Read);
    bgfx::setImage(4, mDensityAdvectedPages, 0, bgfx::Access::Write);
    bgfx::setBuffer(5, mBufferPages[0], bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mAdvectPagedCSProgram, mDispatchIndirect[0]);

    // advect velocity
    bgfx::setImage(0, mVelocityPages, 0, bgfx::Access::Read);
    bgfx::setImage(1, mVelocityPages, 0, bgfx::Access::Read);
    bgfx::setImage(2, mWorldToPages[0], 0, bgfx::Access::Read);
    bgfx::setBuffer(3, mBufferAddressPages[0], bgfx::Access::Read);
    bgfx::setImage(4, mVelocityAdvectedPages, 0, bgfx::Access::Write);
    bgfx::setBuffer(5, mBufferPages[0], bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mAdvectPagedCSProgram, mDispatchIndirect[0]);
    
    // free pages
    bgfx::setBuffer(0, mBufferAddressPages[0], bgfx::Access::Read);
    bgfx::setBuffer(1, mBufferPages[0], bgfx::Access::Read);
    bgfx::setImage(2, mWorldToPageTags, 0, bgfx::Access::Write);
    bgfx::setImage(3, mDensityAdvectedPages, 0, bgfx::Access::Read);
    bgfx::setBuffer(4, mBufferCounter[0], bgfx::Access::ReadWrite);
    bgfx::setBuffer(5, mBufferActivePages, bgfx::Access::ReadWrite);
    bgfx::setBuffer(6, mBufferFreedPages, bgfx::Access::ReadWrite);
    bgfx::setBuffer(7, mBufferActivePageAddresses, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mFreePagesCSProgram, mDispatchIndirect[0]);
    
    // commit pages
    bgfx::setBuffer(0, mBufferPages[0], bgfx::Access::ReadWrite);
    bgfx::setBuffer(1, mBufferCounter[0], bgfx::Access::ReadWrite);
    bgfx::setBuffer(2, mBufferAddressPages[0], bgfx::Access::ReadWrite);
    bgfx::setBuffer(3, mBufferActivePages, bgfx::Access::ReadWrite);
    bgfx::setBuffer(4, mBufferFreedPages, bgfx::Access::ReadWrite);
    bgfx::setBuffer(5, mBufferActivePageAddresses, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mCommitFreePagesCSProgram, 1, 1);
    
    // Dilate pages
    bgfx::setBuffer(0, mBufferAddressPages[0], bgfx::Access::Write);
    bgfx::setImage(1, mWorldToPages[0], 0, bgfx::Access::Write);
    bgfx::setBuffer(2, mBufferPages[0], bgfx::Access::Write);
    bgfx::setBuffer(3, mBufferCounter[0], bgfx::Access::ReadWrite);
    bgfx::setImage(4, mWorldToPageTags, 0, bgfx::Access::Write);
    bgfx::setImage(5, mVelocityAdvectedPages, 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), mDilatePagesCSProgram, (256 / 16) / 16, (256 / 16) / 16); 
    
    // dispatch indirect
    bgfx::setBuffer(0, mDispatchIndirect[0], bgfx::Access::ReadWrite);
    bgfx::setBuffer(1, mBufferCounter[0], bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mDispatchIndirectCSProgram, 1, 1);
    
    // Divergence
    bgfx::setImage(0, mVelocityAdvectedPages, 0, bgfx::Access::Read);
    bgfx::setImage(2, mWorldToPages[0], 0, bgfx::Access::Read);
    bgfx::setBuffer(3, mBufferAddressPages[0], bgfx::Access::Read);
    bgfx::setImage(4, mDivergencePages, 0, bgfx::Access::Write);
    bgfx::setBuffer(5, mBufferPages[0], bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mDivergencePagedCSProgram, mDispatchIndirect[0]);
    
    // VCycle
    VCycle(textureProvider, 0, 1);
 
    // -------------------------------------------
    // 
    // gradient
    bgfx::setImage(0, mJacobiPages[0], 0, bgfx::Access::Read);
    bgfx::setImage(1, mVelocityAdvectedPages, 0, bgfx::Access::Read);
    bgfx::setImage(2, mWorldToPages[0], 0, bgfx::Access::Read);
    bgfx::setBuffer(3, mBufferAddressPages[0], bgfx::Access::Read);
    bgfx::setImage(4, mGradientPages, 0, bgfx::Access::Write);
    bgfx::setBuffer(5, mBufferPages[0], bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mGradientPagedCSProgram, mDispatchIndirect[0]);

    std::swap(mGradientPages, mVelocityPages);
    std::swap(mDensityPages, mDensityAdvectedPages);
}

void TGPU::VCycle(TextureProvider& textureProvider, int level, int maxLevel)
{
    const float hsq = float(level + 1);
    int ssteps = 4;
    int steps = 50;
    
    if (level == maxLevel)
    {
        ClearPages(textureProvider, mJacobiPages[level], mBufferPages[level], mDispatchIndirect[level]);
        Jacobi(textureProvider, mJacobiPages[level], mResidualPages[level], mWorldToPages[level], mBufferPages[level], mBufferAddressPages[level], mDispatchIndirect[level], hsq, steps);

        return;
    }

    bgfx::TextureHandle rhs = mDivergencePages;

    ClearPages(textureProvider, mJacobiPages[0], mBufferPages[level], mDispatchIndirect[level]);
    Jacobi(textureProvider, mJacobiPages[0], rhs, mWorldToPages[level], mBufferPages[level], mBufferAddressPages[level], mDispatchIndirect[level], hsq, ssteps);

    // Residual
    ComputeResidual(textureProvider, mJacobiPages[0], rhs, mWorldToPages[level], mResidualPages[level], mBufferPages[level], mBufferAddressPages[level], mDispatchIndirect[level], hsq);

    // next level -------------------------------

    // init
    float pageCount[4] = { 63.f, 0.f, 0.f, 0.f };
    bgfx::setUniform(mInitPageCountUniform, pageCount);
    bgfx::setBuffer(0, mBufferPages[level + 1], bgfx::Access::Write);
    bgfx::setBuffer(1, mBufferCounter[level + 1], bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mInitPagesCSProgram, 1, 1);

    // new indirection map
    bgfx::setBuffer(0, mBufferAddressPages[level + 1], bgfx::Access::Write);
    bgfx::setImage(1, mWorldToPages[level + 1], 0, bgfx::Access::Write);
    bgfx::setBuffer(2, mBufferPages[level + 1], bgfx::Access::ReadWrite);
    bgfx::setBuffer(3, mBufferCounter[level + 1], bgfx::Access::ReadWrite);
    bgfx::setImage(4, mWorldToPageTags, 0, bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mAllocateSubPagesCSProgram, 1, 1);

    // dispatch indirect
    bgfx::setBuffer(0, mDispatchIndirect[level + 1], bgfx::Access::ReadWrite);
    bgfx::setBuffer(1, mBufferCounter[level + 1], bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mDispatchIndirectCSProgram, 1, 1);

    // downscale filter
    bgfx::setImage(0, mResidualPages[level], 0, bgfx::Access::Read);
    bgfx::setImage(2, mWorldToPages[level], 0, bgfx::Access::Read);
    bgfx::setImage(3, mResidualPages[level+1], 0, bgfx::Access::Write);
    bgfx::setBuffer(4, mBufferAddressPages[level + 1], bgfx::Access::Read);
    bgfx::setBuffer(5, mBufferPages[level + 1], bgfx::Access::Read);
    bgfx::dispatch(textureProvider.GetViewId(), mDownscalePagedCSProgram, mDispatchIndirect[level + 1]);
    
    VCycle(textureProvider, level+1, maxLevel);
    
    // upscale level1 -> level0
    bgfx::setImage(0, mJacobiPages[level + 1], 0, bgfx::Access::Read); //unext
    bgfx::setImage(2, mWorldToPages[level + 1], 0, bgfx::Access::Read);
    bgfx::setBuffer(3,mBufferAddressPages[level], bgfx::Access::Read);
    bgfx::setBuffer(4,mBufferPages[level], bgfx::Access::Read);
    bgfx::setImage(5, mJacobiPages[0], 0, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), mUpscalePagedCSProgram, mDispatchIndirect[level]);
    
    Jacobi(textureProvider, mJacobiPages[0], rhs, mWorldToPages[level], mBufferPages[level], mBufferAddressPages[level], mDispatchIndirect[level], hsq, ssteps);
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

    if (ImGui::RadioButton("Density", mDebugDisplay == 0)) mDebugDisplay = 0;
    if (ImGui::RadioButton("Velocity", mDebugDisplay == 1)) mDebugDisplay = 1;
    if (ImGui::RadioButton("Page Tag", mDebugDisplay == 2)) mDebugDisplay = 2;
    if (ImGui::RadioButton("Divergence", mDebugDisplay == 3)) mDebugDisplay = 3;
    const char* const levels[] = {"Level 0", "Level 1", "Level 2", "Level 3"};
    ImGui::Combo("Level", &mDebugLevel, levels, MaxLevel);
    if (ImGui::RadioButton("Jacobi", mDebugDisplay == 4)) mDebugDisplay = 4;
    if (ImGui::RadioButton("Residual", mDebugDisplay == 5)) mDebugDisplay = 5;
    if (ImGui::RadioButton("Gradient", mDebugDisplay == 6)) mDebugDisplay = 6;

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
    case 4: return mJacobiPages[mDebugLevel];
    case 5: return mResidualPages[mDebugLevel];
    case 6: return mGradientPages;
    }
    return { bgfx::kInvalidHandle };
}

bgfx::TextureHandle TGPU::GetDisplayPageIndirection() const
{
    switch (mDebugDisplay)
    {
        case 8:
        case 7: return mWorldToPages[1];
        default: return mWorldToPages[0];
    }
    return { bgfx::kInvalidHandle };
}
