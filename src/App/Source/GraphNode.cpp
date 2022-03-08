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

bool Vorticity::UI()
{
    bool changed = ImGui::InputFloat("Epsilon", &m_epsilon);
    changed |= ImGui::InputFloat("Curl", &m_curl);
    return changed;
}


// -----------------------------------------------------------------------------------------------------


void DensityGen::Init()
{
    m_densityGenCSProgram = App::LoadProgram("DensityGen_cs", nullptr);
    m_positionUniform = bgfx::createUniform("position", bgfx::UniformType::Vec4);

    GraphEditorDelegate::mTemplateFunctions.push_back(DensityGen::GetTemplate);
}

void DensityGen::Tick(TextureProvider& textureProvider)
{
    float position[4] = { m_position.x, m_position.y, m_position.z, m_radius };
    bgfx::setUniform(m_positionUniform, position);

    auto density = m_inputs[0];
    bgfx::setImage(0, density->GetTexture(), 0, bgfx::Access::ReadWrite);
    bgfx::dispatch(5, m_densityGenCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    m_outputs[0] = density;
}

bool DensityGen::UI()
{
    bool changed = ImGui::InputFloat("Radius", &m_radius);
    return changed;
}

// -----------------------------------------------------------------------------------------------------

void VelocityGen::Init()
{
    m_velocityGenCSProgram = App::LoadProgram("VelocityGen_cs", nullptr);
    m_positionUniform = bgfx::createUniform("position", bgfx::UniformType::Vec4);
    m_directionUniform = bgfx::createUniform("direction", bgfx::UniformType::Vec4);

    GraphEditorDelegate::mTemplateFunctions.push_back(VelocityGen::GetTemplate);
}

void VelocityGen::Tick(TextureProvider& textureProvider)
{
    float position[4] = { m_position.x, m_position.y, m_position.z, m_radius };
    bgfx::setUniform(m_positionUniform, position);

    float direction[4] = { m_direction.x, m_direction.y, m_direction.z, 0.f };
    bgfx::setUniform(m_directionUniform, direction);

    auto velocity = m_inputs[0];
    bgfx::setImage(0, velocity->GetTexture(), 0, bgfx::Access::ReadWrite);
    bgfx::dispatch(5, m_velocityGenCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    m_outputs[0] = velocity;
}

bool VelocityGen::UI()
{
    bool changed = ImGui::InputFloat("Radius", &m_radius);
    return changed;
}


// -----------------------------------------------------------------------------------------------------

void Advection::Init()
{
    m_advectCSProgram = App::LoadProgram("Advect_cs", nullptr);
    m_advectionUniform = bgfx::createUniform("advection", bgfx::UniformType::Vec4);
    m_texVelocityUniform = bgfx::createUniform("s_texVelocity", bgfx::UniformType::Sampler);
    m_texAdvectUniform = bgfx::createUniform("s_texAdvect", bgfx::UniformType::Sampler);

    GraphEditorDelegate::mTemplateFunctions.push_back(Advection::GetTemplate);
}

void Advection::Tick(TextureProvider& textureProvider)
{
    float advection[4] = { m_timeScale, m_dissipation, 0.f, 0.f };
    bgfx::setUniform(m_advectionUniform, advection);

    auto velocity = m_inputs[0];
    auto toAdvectTexture = m_inputs[1];
    Texture* advected = textureProvider.Acquire();
    bgfx::setTexture(0, m_texVelocityUniform, velocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setTexture(1, m_texAdvectUniform, toAdvectTexture->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setImage(2, advected->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(5, m_advectCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    m_outputs[0] = advected;
}

bool Advection::UI()
{
    bool changed = ImGui::InputFloat("Time scale", &m_timeScale);
    changed |= ImGui::InputFloat("Dissipation", &m_dissipation);
    return changed;
}
