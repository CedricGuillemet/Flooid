#include <bgfx/bgfx.h>
#include "Renderer.h"
#include "Shaders.h"
#include "TextureProvider.h"

bgfx::VertexLayout Renderer::Vertex::ms_layout;

void Renderer::Init()
{
    Vertex::Init();

    static Vertex quadVertices[] =
    {
        {-1.f, 2.f, 0.f,  0.0f,  0.0f },
        { 1.f, 2.f, 0.f,  1.0f,  0.0f },
        {-1.f, 0.f, 0.f,  0.0f,  1.0f },
        { 1.f, 0.f, 0.f,  1.0f,  1.0f },
    };
    static const uint16_t quadIndices[] =
    {
        0,  2,  1,
        1,  2,  3,
    };
    
    m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(quadVertices, sizeof(quadVertices)), Vertex::ms_layout);
    m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(quadIndices, sizeof(quadIndices) ) );


    static Vertex groundVertices[] =
    {
        {-1.0f, 0.f,  1.0f, -1.0f,  1.0f },
        { 1.0f, 0.f,  1.0f,  1.0f,  1.0f },
        {-1.0f, 0.f, -1.0f, -1.0f, -1.0f },
        { 1.0f, 0.f, -1.0f,  1.0f, -1.0f },
    };
    static const uint16_t groundIndices[] =
    {
        0,  2,  1,
        1,  2,  3,
    };

    m_vbhGround = bgfx::createVertexBuffer(bgfx::makeRef(groundVertices, sizeof(groundVertices)), Vertex::ms_layout);
    m_ibhGround = bgfx::createIndexBuffer(bgfx::makeRef(groundIndices, sizeof(groundIndices)));

    m_viewProjectionUniform = bgfx::createUniform("u_viewProjection", bgfx::UniformType::Mat4);
    //m_viewUniform = bgfx::createUniform("view", bgfx::UniformType::Vec4);
    m_renderProgram = App::LoadProgram("Render_vs", "Render_fs");
    m_groundProgram = App::LoadProgram("Ground_vs", "Ground_fs");
    m_texDensityUniform = bgfx::createUniform("s_texDensity", bgfx::UniformType::Sampler);
}

void Renderer::Render(Texture* texture)
{
    const uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LEQUAL;
    Imm::matrix vp = m_camera.GetViewProjection();
    bgfx::setUniform(m_viewProjectionUniform, vp.m16);

    bgfx::setVertexBuffer(0, m_vbhGround);
    bgfx::setIndexBuffer(m_ibhGround);
    bgfx::setState(state);
    bgfx::submit(0, m_groundProgram);


    bgfx::setTexture(0, m_texDensityUniform, texture->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);
    bgfx::setState(state | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
    bgfx::submit(0, m_renderProgram);

    
}
