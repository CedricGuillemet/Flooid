#pragma once
#include "Camera.h"
#include "GraphNode.h"

struct Texture;

class Renderer
{
public:
    Renderer() : m_camera(bgfx::getCaps()->homogeneousDepth, 3.3f, 0.f) {}
    void Init();
    
    void Input(float dx, float dy) { m_camera.Input(dx, dy); }
    void SetDisplaySize(uint16_t width, uint16_t height) { m_camera.SetDisplaySize(width, height); }
    void Render(TextureProvider& textureProvider, Texture* texture, Display* displayNode);
    void Render(TextureProvider& textureProvider, bgfx::TextureHandle texture, Display* displayNode);
    void Tick() { m_camera.ComputeMatrices(); }
    const Camera& GetCamera() const { return m_camera; }
    struct Vertex
    {
        float x,y,z;
        float nx, ny, nz;
        float u, v;
        static void Init()
        {
            ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .end();
        }

        static bgfx::VertexLayout ms_layout;
    };

    void RenderBackground(TextureProvider& textureProvider);
    void Render(TextureProvider& textureProvider, bgfx::TextureHandle textureWorldToTiles, bgfx::TextureHandle tiles, bgfx::TextureHandle densityTiles, bgfx::TextureHandle textureWorldToTileTags, Display* displayNode);
    
    bgfx::TextureHandle GetDebugSlice() const { return bgfx::getTexture(m_debugRenderTarget); }
    void SetDebugSlice(float slice)
    {
        float debugDisplay2[4] = {slice, 0.f, 0.f, 0.f};
        bgfx::setUniform(mDebugDisplayUniform, debugDisplay2);
    }
private:
    Camera m_camera;
    
    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle m_ibh;
    bgfx::VertexBufferHandle m_vbhCube;
    bgfx::IndexBufferHandle m_ibhCube;

    bgfx::VertexBufferHandle m_vbhGround;
    bgfx::IndexBufferHandle m_ibhGround;
    bgfx::UniformHandle m_viewProjectionUniform;
    bgfx::UniformHandle m_texDensityUniform;
    bgfx::UniformHandle m_eyePositionUniform;
    bgfx::UniformHandle m_directionalUniform;

    bgfx::ProgramHandle m_renderProgram;
    bgfx::ProgramHandle m_renderVolumeProgram;
    bgfx::ProgramHandle m_groundProgram;


    bgfx::UniformHandle mTexDensityTiles; // s_texDensityPages
    bgfx::UniformHandle mTexWorldToTile; // s_texWorldToPage
    bgfx::UniformHandle mTexTilesUniform;
    bgfx::UniformHandle mTexWorldToTileTag;


    bgfx::UniformHandle m_texTileUniform;
    bgfx::UniformHandle m_texWorldToTileUniform;
    bgfx::UniformHandle mDebugDisplayUniform;
    
    bgfx::FrameBufferHandle m_debugRenderTarget;
};
