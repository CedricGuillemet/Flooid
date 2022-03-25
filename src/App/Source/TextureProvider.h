#pragma once
#include <vector>
#include <bgfx/bgfx.h>
#include "GraphNode.h"

struct Texture
{
    Texture();
    //bgfx::FrameBufferHandle m_renderTarget;
    bgfx::TextureHandle m_texture;
    bgfx::TextureHandle GetTexture();
    PlugType::Enum m_type{PlugType::Invalid};
};

class TextureProvider
{
public:
    TextureProvider() {}
    ~TextureProvider();
    
    void Init()
    {
        m_densityTexture = Acquire(PlugType::Particles);
        m_velocityTexture = Acquire(PlugType::Velocity);
    }
    
    void TickFrame(bgfx::ViewId viewId);
    Texture* Acquire(PlugType::Enum type);
    void Release(Texture* texture);
    
    // persistent datas
    Texture* m_densityTexture{};
    Texture* m_velocityTexture{};

    
    bgfx::ViewId GetViewId() const { return m_viewId; }
private:
    std::vector<Texture*> m_available;
    std::vector<Texture*> m_inUse;
    bgfx::ViewId m_viewId;
};
