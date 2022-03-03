#include <bgfx/bgfx.h>
#include "Renderer.h"
#include "Shaders.h"

bgfx::VertexLayout Renderer::Vertex::ms_layout;

void Renderer::Init()
{
    Vertex::Init();

    static Vertex quadVertices[] =
    {
        {-1.f, 2.f, 0.f, -1.0f,  1.0f },
        { 1.f, 2.f, 0.f,  1.0f,  1.0f },
        {-1.f, 0.f, 0.f, -1.0f, -1.0f },
        { 1.f, 0.f, 0.f,  1.0f, -1.0f },
    };
    static const uint16_t quadIndices[] =
    {
        0,  2,  1,
        1,  2,  3,
    };
    
    m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(quadVertices, sizeof(quadVertices)), Vertex::ms_layout);
    m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(quadIndices, sizeof(quadIndices) ) );

    m_viewProjectionUniform = bgfx::createUniform("u_viewProjection", bgfx::UniformType::Mat4);
    //m_viewUniform = bgfx::createUniform("view", bgfx::UniformType::Vec4);
    m_renderProgram = App::LoadProgram("Render_vs", "Render_fs");
}

void Renderer::Render()
{
    const uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z;
    Imm::matrix vp = m_camera.GetViewProjection();
    bgfx::setUniform(m_viewProjectionUniform, vp.m16);
    
    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);
    bgfx::setState(state);
    bgfx::submit(0, m_renderProgram);
}
