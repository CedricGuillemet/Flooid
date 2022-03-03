#pragma once
#include "Camera.h"

class Renderer
{
public:
    Renderer() {}
    void Init();
    
    void Render();
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

    bgfx::UniformHandle m_viewProjectionUniform;
    //bgfx::UniformHandle m_viewUniform;

    bgfx::ProgramHandle m_renderProgram;

};
