#include "common.h"
#include "bgfx/bgfx.h"
#include "Flooid.h"
#include <stdint.h>
#include "Shaders.h"

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

    
    m_RT1 = bgfx::createFrameBuffer(TEX_SIZE, TEX_SIZE, bgfx::TextureFormat::RG16F);

    
    m_brushUniform = bgfx::createUniform("brush", bgfx::UniformType::Vec4);
    m_brushDirectionUniform = bgfx::createUniform("brushDirection", bgfx::UniformType::Vec4);
    m_brushColorUniform = bgfx::createUniform("brushColor", bgfx::UniformType::Vec4);
    m_alphaUniform = bgfx::createUniform("alpha", bgfx::UniformType::Vec4);
    m_betaUniform = bgfx::createUniform("beta", bgfx::UniformType::Vec4);
    
    m_texVelocityUniform = bgfx::createUniform("s_texVelocity", bgfx::UniformType::Sampler);
    m_texAdvectUniform = bgfx::createUniform("s_texAdvect", bgfx::UniformType::Sampler);
    m_texColorUniform = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    m_texPressureUniform = bgfx::createUniform("s_texPressure", bgfx::UniformType::Sampler);

    m_renderRTProgram = App::LoadProgram("Quad_vs", "RenderRT_fs");
    m_advectProgram = App::LoadProgram("Quad_vs", "Advect_fs");
    m_divergenceProgram = App::LoadProgram("Quad_vs", "Divergence_fs");
    m_gradientProgram = App::LoadProgram("Quad_vs", "Gradient_fs");
    m_jacobiProgram = App::LoadProgram("Quad_vs", "Jacobi_fs");
    m_paintDensityProgram = App::LoadProgram("Quad_vs", "PaintDensity_fs");
    m_paintVelocityProgram = App::LoadProgram("Quad_vs", "PaintVelocity_fs");
}

void Flooid::Tick(const Parameters& parameters)
{
    const uint64_t state = BGFX_STATE_WRITE_MASK | BGFX_STATE_DEPTH_TEST_ALWAYS;

    // paint density
    float brush[4] = { parameters.x, parameters.y, 0.2f, parameters.lButDown ? 0.5f : 0.f};
    bgfx::setUniform(m_brushUniform, brush);
    float brushColor[4] = { 1.f, 1.f, 1.f, 1.f };
    bgfx::setUniform(m_brushColorUniform, brushColor);

    bgfx::setViewFrameBuffer(1, m_RT1);
    bgfx::setViewRect(1, 0, 0, uint16_t(TEX_SIZE), uint16_t(TEX_SIZE));

    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);
    bgfx::setState(state | BGFX_STATE_BLEND_ADD);
    bgfx::submit(1, m_paintDensityProgram);

    // draw RT
    
    //bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));
    //bgfx::setViewFrameBuffer
    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);
    bgfx::setState(state);
    bgfx::setTexture(0, m_texColorUniform, bgfx::getTexture(m_RT1));
    bgfx::submit(0, m_renderRTProgram);
}
