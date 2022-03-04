#pragma once
#include "Camera.h"

struct Texture;

class Renderer
{
public:
    Renderer() : m_camera(bgfx::getCaps()->homogeneousDepth) {}
    void Init();
    
    void Input(float dx, float dy) { m_camera.Input(dx, dy); }
    void SetDisplaySize(uint16_t width, uint16_t height) { m_camera.SetDisplaySize(width, height); }
    void Render(Texture* texture);
private:
    Camera m_camera;
    
    
    struct Vertex
    {
        float x,y,z;
        float u, v;
        static void Init()
        {
            ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .end();
        }

        static bgfx::VertexLayout ms_layout;
    };
    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle m_ibh;
    bgfx::VertexBufferHandle m_vbhGround;
    bgfx::IndexBufferHandle m_ibhGround;
    bgfx::UniformHandle m_viewProjectionUniform;
    bgfx::UniformHandle m_texDensityUniform;
    //bgfx::UniformHandle m_viewUniform;

    bgfx::ProgramHandle m_renderProgram;
    bgfx::ProgramHandle m_groundProgram;

};
