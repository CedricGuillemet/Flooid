#include "common.h"
#include "bgfx/bgfx.h"
#include "Flooid.h"
#include <stdint.h>
#include "Shaders.h"

/*
 - texture allocator
 - compute shader : test only 1 viewid per node
 - node based solving
 - viscosity node
 - vorticity node
 - generator node
 - speed node
 - paint density node
 - paint speed node
 - solver nodes
 - display node
 - gizmo
 - rendering
 - 3D / camera
 - node based graphics editing
 - save/load json
 */

bgfx::VertexLayout Flooid::QuadVertex::ms_layout;
Flooid::Flooid()
{
}
const int TEX_SIZE = 256;
void Flooid::Init()
{
    QuadVertex::Init();

    static QuadVertex quadVertices[4] =
    {
        {-1.0f,  1.0f },
        { 1.0f,  1.0f },
        {-1.0f, -1.0f },
        { 1.0f, -1.0f },
    };
    static const uint16_t quadIndices[36] =
    {
        0,  2,  1,
        1,  2,  3,
    };

    m_vbh = bgfx::createVertexBuffer(
                bgfx::makeRef(quadVertices, sizeof(quadVertices) )
            , QuadVertex::ms_layout
            );

    m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(quadIndices, sizeof(quadIndices) ) );

    const auto texFormat = bgfx::TextureFormat::RG16F;
    //const auto texFormat = bgfx::TextureFormat::RGBA32F;
    m_RT1 = bgfx::createFrameBuffer(TEX_SIZE, TEX_SIZE, texFormat, BGFX_TEXTURE_COMPUTE_WRITE | BGFX_TEXTURE_RT);
    m_RT2 = bgfx::createFrameBuffer(TEX_SIZE, TEX_SIZE, texFormat, BGFX_TEXTURE_COMPUTE_WRITE | BGFX_TEXTURE_RT);

    m_RT1adv = bgfx::createFrameBuffer(TEX_SIZE, TEX_SIZE, texFormat, BGFX_TEXTURE_COMPUTE_WRITE | BGFX_TEXTURE_RT);
    m_RT2adv = bgfx::createFrameBuffer(TEX_SIZE, TEX_SIZE, texFormat, BGFX_TEXTURE_COMPUTE_WRITE | BGFX_TEXTURE_RT);

    m_RTdivergence = bgfx::createFrameBuffer(TEX_SIZE, TEX_SIZE, texFormat, BGFX_TEXTURE_COMPUTE_WRITE | BGFX_TEXTURE_RT);
    m_RTjacobi[0] = bgfx::createFrameBuffer(TEX_SIZE, TEX_SIZE, texFormat, BGFX_TEXTURE_COMPUTE_WRITE | BGFX_TEXTURE_RT);
    m_RTjacobi[1] = bgfx::createFrameBuffer(TEX_SIZE, TEX_SIZE, texFormat, BGFX_TEXTURE_COMPUTE_WRITE | BGFX_TEXTURE_RT);
    
    
    m_brushUniform = bgfx::createUniform("brush", bgfx::UniformType::Vec4);
    m_brushDirectionUniform = bgfx::createUniform("brushDirection", bgfx::UniformType::Vec4);
    m_brushColorUniform = bgfx::createUniform("brushColor", bgfx::UniformType::Vec4);
    m_jacobiParametersUniform = bgfx::createUniform("jacobiParameters", bgfx::UniformType::Vec4);
    m_advectionUniform = bgfx::createUniform("advection", bgfx::UniformType::Vec4);

    
    m_texVelocityUniform = bgfx::createUniform("s_texVelocity", bgfx::UniformType::Sampler);
    m_texAdvectUniform = bgfx::createUniform("s_texAdvect", bgfx::UniformType::Sampler);
    m_texColorUniform = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    m_texPressureUniform = bgfx::createUniform("s_texPressure", bgfx::UniformType::Sampler);
    m_texDensityUniform = bgfx::createUniform("s_texDensity", bgfx::UniformType::Sampler);
    m_texJacoviUniform = bgfx::createUniform("s_texJacobi", bgfx::UniformType::Sampler);
    m_texDivergenceUniform = bgfx::createUniform("s_texDivergence", bgfx::UniformType::Sampler);

    m_renderRTProgram = App::LoadProgram("Quad_vs", "RenderRT_fs");
    m_paintDensityProgram = App::LoadProgram("Quad_vs", "PaintDensity_fs");
    m_paintVelocityProgram = App::LoadProgram("Quad_vs", "PaintVelocity_fs");

    m_jacobiCSProgram = App::LoadProgram("Jacobi_cs", nullptr);
    m_divergenceCSProgram = App::LoadProgram("Divergence_cs", nullptr);
    m_gradientCSProgram = App::LoadProgram("Gradient_cs", nullptr);
    m_advectCSProgram = App::LoadProgram("Advect_cs", nullptr);
}

void Flooid::Tick(const Parameters& parameters)
{
    const uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;

    // uniforms
    float brushColor[4] = { 1.f, 1.f, 1.f, 1.f };
    bgfx::setUniform(m_brushColorUniform, brushColor);
    float brushDirection[4] = {parameters.dx, parameters.dy, 0.f, 0.f};
    bgfx::setUniform(m_brushDirectionUniform, brushDirection);
    float advection[4] = {1.f, 1.f, 1.f, 1.f};
    bgfx::setUniform(m_advectionUniform, advection);

    // jacobi
    float jacobiParameters[4] = { -1.f, 4.f, 0.f, 0.f };
    bgfx::setUniform(m_jacobiParametersUniform, jacobiParameters);
    
    // paint density
    float brushDensity[4] = { parameters.x, parameters.y, 0.1f, parameters.lButDown ? 0.1f : 0.f };
    bgfx::setUniform(m_brushUniform, brushDensity);

    bgfx::setViewFrameBuffer(1, m_RT1);
    bgfx::setViewRect(1, 0, 0, uint16_t(TEX_SIZE), uint16_t(TEX_SIZE));
    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);
    bgfx::setState(state | BGFX_STATE_BLEND_ADD);
    bgfx::submit(1, m_paintDensityProgram);
    
    // paint velocity
    float brushVelocity[4] = { parameters.x, parameters.y, 0.1f, parameters.rButDown ? 0.5f : 0.f };
    bgfx::setUniform(m_brushUniform, brushVelocity);
    bgfx::setViewFrameBuffer(2, m_RT2);
    bgfx::setViewRect(2, 0, 0, uint16_t(TEX_SIZE), uint16_t(TEX_SIZE));
    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);
    bgfx::setState(state | BGFX_STATE_BLEND_ADD);
    bgfx::submit(2, m_paintVelocityProgram);
    
    // bunch of CS
    bgfx::setViewFrameBuffer(5, { bgfx::kInvalidHandle });

    // advect paint
    bgfx::setTexture(0, m_texVelocityUniform, bgfx::getTexture(m_RT2), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setTexture(1, m_texAdvectUniform, bgfx::getTexture(m_RT1), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setImage(2, bgfx::getTexture(m_RT1adv), 0, bgfx::Access::Write);
    bgfx::dispatch(5, m_advectCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    // advect velocity
    bgfx::setTexture(0, m_texVelocityUniform, bgfx::getTexture(m_RT2), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setTexture(1, m_texAdvectUniform, bgfx::getTexture(m_RT2), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setImage(2, bgfx::getTexture(m_RT2adv), 0, bgfx::Access::Write);
    bgfx::dispatch(5, m_advectCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    // divergence
    bgfx::setTexture(0, m_texVelocityUniform, bgfx::getTexture(m_RT2adv), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(1, bgfx::getTexture(m_RTdivergence), 0, bgfx::Access::Write);
    bgfx::dispatch(5, m_divergenceCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    // clear density
    bgfx::setViewFrameBuffer(6, m_RTjacobi[0]);
    bgfx::setViewRect(6, 0, 0, uint16_t(TEX_SIZE), uint16_t(TEX_SIZE));
    bgfx::setViewClear(6, BGFX_CLEAR_COLOR, 0x00000000);
    bgfx::touch(6);

    // jacobi
    for(int i = 0; i < parameters.m_iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;
        
        bgfx::setTexture(0, m_texJacoviUniform, bgfx::getTexture(m_RTjacobi[indexSource]), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setTexture(1, m_texDivergenceUniform, bgfx::getTexture(m_RTdivergence), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setImage(2, bgfx::getTexture(m_RTjacobi[indexDestination]), 0, bgfx::Access::Write);
        bgfx::dispatch(6, m_jacobiCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    }
    const int lastJacobiIndex = parameters.m_iterationCount & 1;

    // gradient
    bgfx::setViewFrameBuffer(7, { bgfx::kInvalidHandle });
    bgfx::setTexture(0, m_texPressureUniform, bgfx::getTexture(m_RTjacobi[lastJacobiIndex]), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setTexture(1, m_texVelocityUniform, bgfx::getTexture(m_RT2adv), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(2, bgfx::getTexture(m_RT2), 0, bgfx::Access::Write);
    bgfx::dispatch(7, m_gradientCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    
    // draw RT
    //bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));
    bgfx::setViewFrameBuffer(0, {bgfx::kInvalidHandle});
    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);
    bgfx::setState(state);
    switch (parameters.m_display)
    {
    case 0:
        bgfx::setTexture(0, m_texColorUniform, bgfx::getTexture(m_RT1));
        break;
    case 1:
        bgfx::setTexture(0, m_texColorUniform, bgfx::getTexture(m_RT2));
        break;
    case 2:
        bgfx::setTexture(0, m_texColorUniform, bgfx::getTexture(m_RTdivergence));
        break;
    case 3:
        bgfx::setTexture(0, m_texColorUniform, bgfx::getTexture(m_RTjacobi[lastJacobiIndex]));
        break;

    }
    
    bgfx::submit(0, m_renderRTProgram);
    
    // swap advect/vel
    bx::swap(m_RT1, m_RT1adv);
    //bx::swap(m_RT2, m_RT2adv);
}
