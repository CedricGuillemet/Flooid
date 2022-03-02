#include "TextureProvider.h"

Texture::Texture()
{
    const int TEX_SIZE = 256;
    const auto texFormat = bgfx::TextureFormat::RG16F;
    m_renderTarget = bgfx::createFrameBuffer(TEX_SIZE, TEX_SIZE, texFormat, BGFX_TEXTURE_COMPUTE_WRITE | BGFX_TEXTURE_RT);
}

bgfx::TextureHandle Texture::GetTexture()
{
    return bgfx::getTexture(m_renderTarget);
}

void Texture::BindAsTarget(bgfx::ViewId viewId)
{
    bgfx::setViewFrameBuffer(viewId, m_renderTarget);
}

Texture* TextureProvider::Acquire()
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
