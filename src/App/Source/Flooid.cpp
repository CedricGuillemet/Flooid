#include "common.h"
#include "bgfx/bgfx.h"
#include "Flooid.h"
#include <stdint.h>
#include "Shaders.h"
#include "GraphNode.h"

/*
 - display parameters UI
 - texture provider get clearer RT + provide viewId
 - handle gizmo for gens
 
 - solver nodes
 - display node

 - node based solving
 - graph with links
 - graph resolution
 - raymarching lighting / rendering
 
 - node based graphics editing
 - save/load json
 */

Flooid::Flooid()
: m_graph{}
, m_graphEditorDelegate(m_graph)
{
}

const int TEX_SIZE = 256;
void Flooid::Init()
{
    m_renderer.Init();
    
    m_densityTexture = m_textureProvider.Acquire();
    m_velocityTexture = m_textureProvider.Acquire();
    
    m_jacobiParametersUniform = bgfx::createUniform("jacobiParameters", bgfx::UniformType::Vec4);

    m_texVelocityUniform = bgfx::createUniform("s_texVelocity", bgfx::UniformType::Sampler);
    m_texColorUniform = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    m_texPressureUniform = bgfx::createUniform("s_texPressure", bgfx::UniformType::Sampler);
    m_texJacoviUniform = bgfx::createUniform("s_texJacobi", bgfx::UniformType::Sampler);
    m_texDivergenceUniform = bgfx::createUniform("s_texDivergence", bgfx::UniformType::Sampler);
    m_texVorticityUniform = bgfx::createUniform("s_texVorticity", bgfx::UniformType::Sampler);

    m_jacobiCSProgram = App::LoadProgram("Jacobi_cs", nullptr);
    m_divergenceCSProgram = App::LoadProgram("Divergence_cs", nullptr);
    m_gradientCSProgram = App::LoadProgram("Gradient_cs", nullptr);

    Vorticity::Init();
    VelocityGen::Init();
    DensityGen::Init();
    Advection::Init();
    
    m_vorticityNode = new Vorticity;
    m_graph.AddNode(m_vorticityNode);

    m_velocityGenNode = new VelocityGen;
    m_graph.AddNode(m_velocityGenNode);

    m_densityGenNode = new DensityGen;
    m_graph.AddNode(m_densityGenNode);

    m_advectDensity = new Advection;
    m_graph.AddNode(m_densityGenNode);

    m_advectVelocity = new Advection;
    m_graph.AddNode(m_densityGenNode);
}

void Flooid::Tick(const Parameters& parameters)
{
    if (parameters.lButDown)
    {
        m_renderer.Input(parameters.dx, parameters.dy);
    }
    const uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;

    // jacobi
    float jacobiParameters[4] = { -1.f, 4.f, 0.f, 0.f };
    bgfx::setUniform(m_jacobiParametersUniform, jacobiParameters);
    
    // bunch of CS
    bgfx::setViewFrameBuffer(5, { bgfx::kInvalidHandle });

    // advect density
    m_advectDensity->SetInput(0, m_velocityTexture);
    m_advectDensity->SetInput(1, m_densityTexture);
    m_advectDensity->Tick(m_textureProvider);
    Texture* advectedDensity = m_advectDensity->GetOutput(0);
    
    // advect velocity
    m_advectVelocity->SetInput(0, m_velocityTexture);
    m_advectVelocity->SetInput(1, m_velocityTexture);
    m_advectVelocity->Tick(m_textureProvider);
    Texture* advectedVelocity = m_advectVelocity->GetOutput(0);

    // density gen
    m_densityGenNode->SetInput(0, advectedDensity);
    m_densityGenNode->Tick(m_textureProvider);
    advectedDensity = m_densityGenNode->GetOutput(0);

    // velocity gen
    m_velocityGenNode->SetInput(0, advectedVelocity);
    m_velocityGenNode->Tick(m_textureProvider);
    advectedVelocity = m_velocityGenNode->GetOutput(0);

    // vorticity
    if (1)
    {
        m_vorticityNode->SetInput(0, advectedVelocity);
        m_vorticityNode->Tick(m_textureProvider);
        advectedVelocity = m_vorticityNode->GetOutput(0);
    }

    // divergence
    Texture* divergence = m_textureProvider.Acquire();
    bgfx::setTexture(0, m_texVelocityUniform, advectedVelocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(1, divergence->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(5, m_divergenceCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    // clear density
    Texture* jacobi[2] = {m_textureProvider.Acquire(), m_textureProvider.Acquire()};
    jacobi[0]->BindAsTarget(6);
    bgfx::setViewClear(6, BGFX_CLEAR_COLOR, 0x00000000);
    bgfx::touch(6);

    bgfx::setViewFrameBuffer(7, {bgfx::kInvalidHandle});
    
    // jacobi
    for(int i = 0; i < parameters.m_iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;
        
        bgfx::setTexture(0, m_texJacoviUniform, jacobi[indexSource]->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setTexture(1, m_texDivergenceUniform, divergence->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setImage(2, jacobi[indexDestination]->GetTexture(), 0, bgfx::Access::Write);
        bgfx::dispatch(7, m_jacobiCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    }
    const int lastJacobiIndex = parameters.m_iterationCount & 1;
    m_textureProvider.Release(divergence);
    
    // gradient
    bgfx::setTexture(0, m_texPressureUniform, jacobi[lastJacobiIndex]->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setTexture(1, m_texVelocityUniform, advectedVelocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(2, m_velocityTexture->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(7, m_gradientCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    
    m_textureProvider.Release(jacobi[0]);
    m_textureProvider.Release(jacobi[1]);
    m_textureProvider.Release(advectedVelocity);
    
    m_renderer.Render(m_densityTexture);

    // swap advect/vel
    m_textureProvider.Release(m_densityTexture);
    m_densityTexture = advectedDensity;
}

void Flooid::CheckUsingUI()
{
    ImGuiIO& io = ImGui::GetIO();
    
    if (ImGui::IsItemHovered() || ImGui::IsWindowHovered())
    {
        m_usingGUI = true;
    }
    else
    {
        if (m_usingGUI && !io.MouseDown[0] && !io.MouseDown[1] && !io.MouseDown[2])
        {
            m_usingGUI = false;
        }
    }
}

bool Flooid::UI()
{
    ImGui::Begin("Graph Editor", NULL, 0);
    if (ImGui::Button("Fit all nodes"))
    {
        m_graphEditorFit = GraphEditor::Fit_AllNodes;
    }
    ImGui::SameLine();
    if (ImGui::Button("Fit selected nodes"))
    {
        m_graphEditorFit = GraphEditor::Fit_SelectedNodes;
    }
    GraphEditor::Show(m_graphEditorDelegate, m_graphEditorOptions, m_graphEditorViewState, true, &m_graphEditorFit);
    CheckUsingUI();
    ImGui::End();
    return m_usingGUI;
}
