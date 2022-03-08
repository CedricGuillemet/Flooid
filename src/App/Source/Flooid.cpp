#include "common.h"
#include "bgfx/bgfx.h"
#include "Flooid.h"
#include <stdint.h>
#include "Shaders.h"
#include "GraphNode.h"

/*
 - display parameters UI : FlooidUI
 - handle gizmo for gens
 - debug display for gen nodes volumes

 - graph with links
 - graph solving with solving list
 - auto arrange nodes
 - raymarching lighting / rendering

 - pan camera

 - CI
 - display node

 - node based graphics editing
 - save/load json

 - expand velocity/density nodes with more primitives
 - perlin noise node
 - LH/RH for imguizmo
 */

Flooid::Flooid()
: m_graph{}
, m_ui(m_graph)
{
}

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
    m_vorticityNode->m_x = 0; m_vorticityNode->m_y = 0;
    m_graph.AddNode(m_vorticityNode);

    m_velocityGenNode = new VelocityGen;
    m_velocityGenNode->m_x = 250; m_velocityGenNode->m_y = 0;
    m_graph.AddNode(m_velocityGenNode);

    m_densityGenNode = new DensityGen;
    m_densityGenNode->m_x = 500; m_densityGenNode->m_y = 0;
    m_graph.AddNode(m_densityGenNode);

    m_advectDensityNode = new Advection;
    m_advectDensityNode->m_x = 0; m_advectDensityNode->m_y = 120;
    m_graph.AddNode(m_advectDensityNode);

    m_advectVelocityNode = new Advection;
    m_advectVelocityNode->m_x = 250; m_advectVelocityNode->m_y = 120;
    m_graph.AddNode(m_advectVelocityNode);

    m_solverNode = new Solver;
    m_solverNode->m_x = 500; m_solverNode->m_y = 120;
    m_graph.AddNode(m_solverNode);
}

void Flooid::Tick(const Parameters& parameters)
{
    if (parameters.lButDown)
    {
        m_renderer.Input(parameters.dx, parameters.dy);
    }

    m_textureProvider.TickInit(6);

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

bool Flooid::UI()
{
    return m_ui.UI();
}
