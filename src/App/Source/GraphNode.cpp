#include "GraphNode.h"
#include "TextureProvider.h"
#include "Shaders.h"
#include "imgui.h"
#include "GraphEditorDelegate.h"

const int TEX_SIZE = 256;

void Vorticity::Init()
{
    m_vorticityCSProgram = App::LoadProgram("Vorticity_cs", nullptr);
    m_vorticityForceCSProgram = App::LoadProgram("VorticityForce_cs", nullptr);
    m_texVelocityUniform = bgfx::createUniform("s_texVelocity", bgfx::UniformType::Sampler);
    m_texVorticityUniform = bgfx::createUniform("s_texVorticity", bgfx::UniformType::Sampler);
    m_curlUniform = bgfx::createUniform("curl", bgfx::UniformType::Vec4);
    m_epsilonUniform = bgfx::createUniform("epsilon", bgfx::UniformType::Vec4);

    GraphEditorDelegate::mTemplateFunctions.push_back(Vorticity::GetTemplate);
}

void Vorticity::Tick(TextureProvider& textureProvider)
{
    float epsilon[4] = { m_epsilon, 0.f, 0.f, 0.f };
    bgfx::setUniform(m_epsilonUniform, epsilon);
    float curl[4] = { m_curl, m_curl, 0.f, 0.f };
    bgfx::setUniform(m_curlUniform, curl);

    auto advectedVelocity = m_inputs[0];
    Texture* vorticity = textureProvider.Acquire();
    bgfx::setTexture(0, m_texVelocityUniform, advectedVelocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(1, vorticity->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(5, m_vorticityCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    Texture* vorticityForce = textureProvider.Acquire();
    bgfx::setTexture(0, m_texVelocityUniform, advectedVelocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setTexture(1, m_texVorticityUniform, vorticity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(2, vorticityForce->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(5, m_vorticityForceCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    textureProvider.Release(vorticity);
    textureProvider.Release(advectedVelocity);
    m_outputs[0] = vorticityForce;
}

bool Vorticity::Edit()
{
    bool changed = ImGui::InputFloat("Epsilon", &m_epsilon);
    changed |= ImGui::InputFloat("Curl", &m_curl);
    return changed;
}
