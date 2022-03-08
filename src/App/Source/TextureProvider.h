#pragma once
#include <vector>
#include <bgfx/bgfx.h>

struct Texture
{
    Texture();
    bgfx::FrameBufferHandle m_renderTarget;
    bgfx::TextureHandle GetTexture();
    void BindAsTarget(bgfx::ViewId viewId);
};

class TextureProvider
{
public:
    TextureProvider() {}
    ~TextureProvider();
    
    void TickInit(bgfx::ViewId viewId);
    Texture* Acquire();
    Texture* AcquireWithClear(uint32_t clearColor);
    void Release(Texture* texture);
    
    bgfx::ViewId GetViewId() const { return m_viewId; }
private:
    std::vector<Texture*> m_available;
    std::vector<Texture*> m_inUse;
    bgfx::ViewId m_viewId;
};
