#include "common.h"
#include "bgfx/bgfx.h"
#include "Flooid.h"
#include <stdint.h>

bgfx::VertexLayout Flooid::QuadVertex::ms_layout;
Flooid::Flooid()
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

    const int TEX_SIZE = 256;
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
}

void Flooid::Tick(const Parameters& parameters)
{
    
}
