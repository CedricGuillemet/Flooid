#include "TextureProvider.h"
#include <algorithm>

const int TEX_SIZE = 256;
Texture::Texture()
{
    const auto texFormat = bgfx::TextureFormat::RGBA16F;
    m_renderTarget = bgfx::createFrameBuffer(TEX_SIZE, TEX_SIZE, texFormat, BGFX_TEXTURE_COMPUTE_WRITE | BGFX_TEXTURE_RT);
    //m_texture = bgfx::createTexture3D(TEX_SIZE, TEX_SIZE, TEX_SIZE, false, texFormat, BGFX_TEXTURE_COMPUTE_WRITE);
}

bgfx::TextureHandle Texture::GetTexture()
{
    return bgfx::getTexture(m_renderTarget);
    //return m_texture;
}

TextureProvider::~TextureProvider()
{
    
}

void TextureProvider::TickFrame(bgfx::ViewId viewId)
{
    m_viewId = viewId;
}

Texture* TextureProvider::Acquire(PlugType::Enum type)
{
    if (!m_available.empty())
    {
        auto iter = m_available.begin();
        Texture* texture = *iter;
        m_available.erase(iter);
        m_inUse.push_back(texture);
        return texture;
    }
    Texture* texture = new Texture();
    printf("Acquire Texture %p (type %d)\n", this, int(type));
    texture->m_type = type;
    m_inUse.push_back(texture);
    return texture;
}

void TextureProvider::Release(Texture* texture)
{
    auto iter = std::find(m_inUse.begin(), m_inUse.end(), texture);
    if (iter != m_inUse.end())
    {
        m_inUse.erase(iter);
        m_available.push_back(texture);
    }
}
