#pragma once
#include <vector>
#include <bgfx/bgfx.h>
#include "GraphNode.h"

struct Texture
{
    Texture(size_t size);
    bgfx::FrameBufferHandle m_renderTarget;
    //bgfx::TextureHandle m_texture;
    bgfx::TextureHandle GetTexture();
    PlugType::Enum m_type{PlugType::Invalid};
    size_t m_size;
};

class TextureProvider
{
public:
    TextureProvider() {}
    ~TextureProvider();
    
    void Init()
    {
        m_densityTexture = Acquire(PlugType::Particles, 256);
        m_velocityTexture = Acquire(PlugType::Velocity, 256);
    }
    
    void TickFrame(bgfx::ViewId viewId);
    Texture* Acquire(PlugType::Enum type, size_t size);
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
