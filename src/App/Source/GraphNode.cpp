#include "GraphNode.h"
#include "TextureProvider.h"
#include "Shaders.h"
#include "imgui.h"
#include "GraphEditorDelegate.h"
#include "UIGizmos.h"

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
    bgfx::dispatch(textureProvider.GetViewId(), m_vorticityCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    Texture* vorticityForce = textureProvider.Acquire();
    bgfx::setTexture(0, m_texVelocityUniform, advectedVelocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setTexture(1, m_texVorticityUniform, vorticity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(2, vorticityForce->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_vorticityForceCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    textureProvider.Release(vorticity);
    textureProvider.Release(advectedVelocity);
    m_outputs[0] = vorticityForce;
}

bool Vorticity::UI(UIGizmos& uiGizmos)
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
    bgfx::dispatch(textureProvider.GetViewId(), m_densityGenCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    m_outputs[0] = density;
}

bool DensityGen::UI(UIGizmos& uiGizmos)
{
    bool changed = ImGui::InputFloat("Radius", &m_radius);
    uiGizmos.EditPositionScale(&m_position, &m_radius);
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
    bgfx::dispatch(textureProvider.GetViewId(), m_velocityGenCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    m_outputs[0] = velocity;
}

bool VelocityGen::UI(UIGizmos& uiGizmos)
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
    bgfx::dispatch(textureProvider.GetViewId(), m_advectCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    m_outputs[0] = advected;
}

bool Advection::UI(UIGizmos& uiGizmos)
{
    bool changed = ImGui::InputFloat("Time scale", &m_timeScale);
    changed |= ImGui::InputFloat("Dissipation", &m_dissipation);
    return changed;
}

// -----------------------------------------------------------------------------------------------------

void Solver::Init()
{
    m_jacobiCSProgram = App::LoadProgram("Jacobi_cs", nullptr);
    m_divergenceCSProgram = App::LoadProgram("Divergence_cs", nullptr);
    m_gradientCSProgram = App::LoadProgram("Gradient_cs", nullptr);
    m_jacobiParametersUniform = bgfx::createUniform("jacobiParameters", bgfx::UniformType::Vec4);
    m_texVelocityUniform = bgfx::createUniform("s_texVelocity", bgfx::UniformType::Sampler);
    m_texJacoviUniform = bgfx::createUniform("s_texJacobi", bgfx::UniformType::Sampler);
    m_texDivergenceUniform = bgfx::createUniform("s_texDivergence", bgfx::UniformType::Sampler);

    GraphEditorDelegate::mTemplateFunctions.push_back(Solver::GetTemplate);
}

void Solver::Tick(TextureProvider& textureProvider)
{
    float jacobiParameters[4] = { -1.f, 4.f, 0.f, 0.f };
    bgfx::setUniform(m_jacobiParametersUniform, jacobiParameters);

    // divergence
    auto* advectedVelocity = m_inputs[0];
    Texture* divergence = textureProvider.Acquire();
    bgfx::setTexture(0, m_texVelocityUniform, advectedVelocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(1, divergence->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_divergenceCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    // clear density
    Texture* jacobi[2] = { textureProvider.AcquireWithClear(0x00000000), textureProvider.Acquire() };

    // jacobi
    for (int i = 0; i < m_iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;

        bgfx::setTexture(0, m_texJacoviUniform, jacobi[indexSource]->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setTexture(1, m_texDivergenceUniform, divergence->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setImage(2, jacobi[indexDestination]->GetTexture(), 0, bgfx::Access::Write);
        bgfx::dispatch(textureProvider.GetViewId(), m_jacobiCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    }
    const int lastJacobiIndex = m_iterationCount & 1;
    textureProvider.Release(divergence);

    // gradient
    Texture* outputVelocity = textureProvider.Acquire();
    bgfx::setTexture(0, m_texPressureUniform, jacobi[lastJacobiIndex]->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setTexture(1, m_texVelocityUniform, advectedVelocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(2, outputVelocity->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_gradientCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    textureProvider.Release(jacobi[0]);
    textureProvider.Release(jacobi[1]);
    textureProvider.Release(advectedVelocity);

    m_outputs[0] = outputVelocity;
}

bool Solver::UI(UIGizmos& uiGizmos)
{
    bool changed = ImGui::InputFloat("Alpha", &m_alpha);
    changed |= ImGui::InputFloat("Beta", &m_beta);
    return changed;
}

// -----------------------------------------------------------------------------------------------------

GraphNode* Graph::GetSelectedNode() const
{
    GraphNode* res{nullptr};
    for(auto node : m_nodes)
    {
        if(!node->m_selected)
        {
            continue;
        }
        if(!res)
        {
            res = node;
        }
        else
        {
            return nullptr;
        }
    }
    return res;
}
