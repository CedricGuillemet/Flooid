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

    Vorticity::Init();
    VelocityGen::Init();
    DensityGen::Init();
    Advection::Init();
    Solver::Init();

    
    
    m_vorticityNode = new Vorticity;
    m_graph.AddNode(m_vorticityNode);

    m_velocityGenNode = new VelocityGen;
    m_graph.AddNode(m_velocityGenNode);

    m_densityGenNode = new DensityGen;
    m_graph.AddNode(m_densityGenNode);

    m_advectDensityNode = new Advection;
    m_graph.AddNode(m_advectDensityNode);

    m_advectVelocityNode = new Advection;
    m_graph.AddNode(m_advectVelocityNode);

    m_solverNode = new Solver;
    m_graph.AddNode(m_solverNode);
}

void Flooid::Tick(const Parameters& parameters)
{
    if (parameters.lButDown)
    {
        m_renderer.Input(parameters.dx, parameters.dy);
    }

    // bunch of CS
    bgfx::setViewFrameBuffer(5, { bgfx::kInvalidHandle });

    // advect density
    m_advectDensityNode->SetInput(0, m_velocityTexture);
    m_advectDensityNode->SetInput(1, m_densityTexture);
    m_advectDensityNode->Tick(m_textureProvider);
    Texture* advectedDensity = m_advectDensityNode->GetOutput(0);
    
    // advect velocity
    m_advectVelocityNode->SetInput(0, m_velocityTexture);
    m_advectVelocityNode->SetInput(1, m_velocityTexture);
    m_advectVelocityNode->Tick(m_textureProvider);
    Texture* advectedVelocity = m_advectVelocityNode->GetOutput(0);

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
    m_solverNode->SetInput(0, advectedVelocity);
    m_solverNode->Tick(m_textureProvider);
    m_textureProvider.Release(m_velocityTexture);
    m_velocityTexture = m_solverNode->GetOutput(0);

    // render 3D
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
