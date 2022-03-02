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
    
    Texture* Acquire();
    void Release(Texture* texture);
    
private:
    std::vector<Texture*> m_available;
    std::vector<Texture*> m_inUse;
};
