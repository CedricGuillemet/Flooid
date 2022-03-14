#include "TextureProvider.h"
#include <algorithm>

const int TEX_SIZE = 256;
Texture::Texture()
{
    const auto texFormat = bgfx::TextureFormat::RG16F;
    m_renderTarget = bgfx::createFrameBuffer(TEX_SIZE, TEX_SIZE, texFormat, BGFX_TEXTURE_COMPUTE_WRITE | BGFX_TEXTURE_RT);
}

bgfx::TextureHandle Texture::GetTexture()
{
    return bgfx::getTexture(m_renderTarget);
}

TextureProvider::~TextureProvider()
{
    
}

void TextureProvider::TickFrame(bgfx::ViewId viewId)
{
    m_viewId = viewId;
}

void Texture::BindAsTarget(bgfx::ViewId viewId)
{
    bgfx::setViewFrameBuffer(viewId, m_renderTarget);
    bgfx::setViewRect(viewId, 0, 0, uint16_t(TEX_SIZE), uint16_t(TEX_SIZE));
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

Texture* TextureProvider::AcquireWithClear(PlugType::Enum type, uint32_t clearColor)
{
    Texture* texture = Acquire(type);
    m_viewId++;
    texture->BindAsTarget(m_viewId);
    bgfx::setViewClear(m_viewId, BGFX_CLEAR_COLOR, 0x00000000);
    bgfx::touch(m_viewId);
    m_viewId++;
    bgfx::setViewFrameBuffer(m_viewId, { bgfx::kInvalidHandle });
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
