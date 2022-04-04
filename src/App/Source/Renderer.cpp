#include <bgfx/bgfx.h>
#include "Renderer.h"
#include "Shaders.h"
#include "TextureProvider.h"

bgfx::VertexLayout Renderer::Vertex::ms_layout;

void GenerateCurvedGrid(std::vector<Renderer::Vertex>& vertices, std::vector<uint16_t>& indices)
{
    const int tesselationSegments = 16;
    const float halfWidth = 20.f;
    const float halfDepth = 20.f;
    const float quarterCircleLength = 0.5f * Imm::PI;
    const float offsetZ = 2.f;
    const uint32_t indexCount = tesselationSegments * 6;
    const uint32_t vertexCount = tesselationSegments * 2 + 2;
    indices.reserve(indexCount);
    vertices.reserve(vertexCount);
    
    for (uint32_t i = 0; i < tesselationSegments; i++)
    {
        indices.push_back(i * 2 + 0);
        indices.push_back(i * 2 + 2);
        indices.push_back(i * 2 + 1);
        
        indices.push_back(i * 2 + 1);
        indices.push_back(i * 2 + 2);
        indices.push_back(i * 2 + 3);
    }
    
    for (uint32_t i = 0; i < vertexCount / 2; i++)
    {
        const float t = float(i) / float(tesselationSegments);
        const float ng = t * Imm::PI * 0.5f + Imm::PI * 1.5f;
        Imm::vec3 n{0.f, -sinf(ng), -cosf(ng)};
        vertices.push_back({-halfWidth, sinf(ng) + 1.f, cosf(ng) + offsetZ, n.x, n.y, n.z, -halfWidth, t * quarterCircleLength});
        vertices.push_back({ halfWidth, sinf(ng) + 1.f, cosf(ng) + offsetZ, n.x, n.y, n.z,  halfWidth, t * quarterCircleLength});
    }
    // ground
    static const int planIndices[] = {0,  2,  1, 1,  2,  3};
    for(auto index : planIndices)
    {
        indices.push_back(static_cast<uint16_t>(index + vertices.size()));
    }
    vertices.push_back({-halfWidth, 0.f,       0.0f + offsetZ, 0.f, 1.f, 0.f, -halfWidth,       0.0f });
    vertices.push_back({ halfWidth, 0.f,       0.0f + offsetZ, 0.f, 1.f, 0.f,  halfWidth,       0.0f });
    vertices.push_back({-halfWidth, 0.f, -halfDepth + offsetZ, 0.f, 1.f, 0.f, -halfWidth, -halfDepth });
    vertices.push_back({ halfWidth, 0.f, -halfDepth + offsetZ, 0.f, 1.f, 0.f,  halfWidth, -halfDepth });

    for(auto index : planIndices)
    {
        indices.push_back(static_cast<uint16_t>(index + vertices.size()));
    }
    
    vertices.push_back({-halfWidth, 1.f,             1.0f + offsetZ, 0.f, 0.f, -1.f, -halfWidth,  quarterCircleLength});
    vertices.push_back({ halfWidth, 1.f,             1.0f + offsetZ, 0.f, 0.f, -1.f,  halfWidth,  quarterCircleLength});
    vertices.push_back({-halfWidth, 1.f + halfDepth, 1.0f + offsetZ, 0.f, 0.f, -1.f, -halfWidth,  quarterCircleLength + halfDepth});
    vertices.push_back({ halfWidth, 1.f + halfDepth, 1.0f + offsetZ, 0.f, 0.f, -1.f,  halfWidth,  quarterCircleLength + halfDepth});
}

void Renderer::Init()
{
    Vertex::Init();

    static Vertex quadVertices[] =
    {
        { 0.f, 1.f, 0.f,  0.f, 0.f, -1.f, 0.0f,  1.0f },
        { 1.f, 1.f, 0.f,  0.f, 0.f, -1.f, 1.0f,  1.0f },
        { 0.f, 0.f, 0.f,  0.f, 0.f, -1.f, 0.0f,  0.0f },
        { 1.f, 0.f, 0.f,  0.f, 0.f, -1.f, 1.0f,  0.0f },
    };
    static const uint16_t quadIndices[] =
    {
        0,  2,  1,
        1,  2,  3,
    };
    
    static Vertex CubeVertices[] =
    {
        { 0.0f,  1.0f,  1.0f,   0.f, 0.f, 1.f,  0.f, 0.f },
        { 1.0f,  1.0f,  1.0f,   0.f, 0.f, 1.f,  0.f, 0.f },
        { 0.0f,  0.0f,  1.0f,   0.f, 0.f, 1.f,  0.f, 0.f },
        { 1.0f,  0.0f,  1.0f,   0.f, 0.f, 1.f,  0.f, 0.f },
        { 0.0f,  1.0f,  0.0f,   0.f, 0.f, 1.f,  0.f, 0.f },
        { 1.0f,  1.0f,  0.0f,   0.f, 0.f, 1.f,  0.f, 0.f },
        { 0.0f,  0.0f,  0.0f,   0.f, 0.f, 1.f,  0.f, 0.f },
        { 1.0f,  0.0f,  0.0f,   0.f, 0.f, 1.f,  0.f, 0.f },
    };
    static const uint16_t CubeIndices[] =
    {
        0, 1, 2, // 0
        1, 3, 2,
        4, 6, 5, // 2
        5, 6, 7,
        0, 2, 4, // 4
        4, 2, 6,
        1, 5, 3, // 6
        5, 7, 3,
        0, 4, 1, // 8
        4, 5, 1,
        2, 3, 6, // 10
        6, 3, 7,
    };

    
    m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(quadVertices, sizeof(quadVertices)), Vertex::ms_layout);
    m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(quadIndices, sizeof(quadIndices) ) );

    m_vbhCube = bgfx::createVertexBuffer(bgfx::makeRef(CubeVertices, sizeof(CubeVertices)), Vertex::ms_layout);
    m_ibhCube = bgfx::createIndexBuffer(bgfx::makeRef(CubeIndices, sizeof(CubeIndices) ) );

    static std::vector<Renderer::Vertex> vertices;
    static std::vector<uint16_t> indices;
    GenerateCurvedGrid(vertices, indices);
    
    m_vbhGround = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), static_cast<uint32_t>(sizeof(Vertex) * vertices.size())), Vertex::ms_layout);
    m_ibhGround = bgfx::createIndexBuffer(bgfx::makeRef(indices.data(), static_cast<uint32_t>(sizeof(uint16_t) * indices.size())));
    
    m_viewProjectionUniform = bgfx::createUniform("u_viewProjection", bgfx::UniformType::Mat4);
    //m_viewUniform = bgfx::createUniform("view", bgfx::UniformType::Vec4);
    m_renderProgram = App::LoadProgram("Render_vs", "Render_fs");
    m_renderVolumeProgram = App::LoadProgram("RenderVolume_vs", "RenderVolume_fs");
    m_groundProgram = App::LoadProgram("Ground_vs", "Ground_fs");
    m_texDensityUniform = bgfx::createUniform("s_texDensity", bgfx::UniformType::Sampler);
    m_eyePositionUniform = bgfx::createUniform("eyePosition", bgfx::UniformType::Vec4);
    m_directionalUniform = bgfx::createUniform("directional", bgfx::UniformType::Vec4);
}

void Renderer::Render(Texture* texture, Display* displayNode)
{
    const uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LEQUAL;
    Imm::matrix vp = m_camera.GetViewProjection();
    bgfx::setUniform(m_viewProjectionUniform, vp.m16);
    
    auto eye = m_camera.GetPosition();
    float eyePosition[4] = {eye.x, eye.y, eye.z, 0.f};
    bgfx::setUniform(m_eyePositionUniform, eyePosition);
    
    Imm::vec3 lightDir = Imm::vec3{0.5f, 0.5f, 0.5f};
    lightDir -= displayNode->GetLightPosition();
    Imm::vec4 dir = Imm::normalized({lightDir.x, lightDir.y, lightDir.z, 0.f});
    float directional[4] = {dir.x, dir.y, dir.z, 0.f};
    bgfx::setUniform(m_directionalUniform, directional);
    

    //bgfx::setTexture(0, m_texDensityUniform, texture->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP |  BGFX_SAMPLER_W_CLAMP);
    bgfx::setVertexBuffer(0, m_vbhGround);
    bgfx::setIndexBuffer(m_ibhGround);
    bgfx::setState(state);
    bgfx::submit(0, m_groundProgram);


    /*
     3D Cube
     
    bgfx::setTexture(0, m_texDensityUniform, texture->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP |  BGFX_SAMPLER_W_CLAMP);
    bgfx::setVertexBuffer(0, m_vbhCube);
    bgfx::setIndexBuffer(m_ibhCube);
    bgfx::setState(state | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
    bgfx::submit(0, m_renderVolumeProgram);
     */
    
    bgfx::setTexture(0, m_texDensityUniform, texture->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP |  BGFX_SAMPLER_W_CLAMP);
    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);
    bgfx::setState(state | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
    bgfx::submit(0, m_renderProgram);
}
