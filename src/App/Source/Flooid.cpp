#include "common.h"
#include "bgfx/bgfx.h"
#include "Flooid.h"
#include <stdint.h>
#include "Shaders.h"
#include "GraphNode.h"

/*
 - linear/saturate for vel/den gen
 - velgen: direction, vortex, inward, outward
 - mix perlin den/vel (perlin modulate)
 
 - test lighting with perlin density
 
 - raymarching lighting / rendering

 - animation
 
 - LH/RH for imguizmo
 - inputs bgfx/imgui

 - save/load json

 - expand velocity/density nodes with more SDF primitives

 - debug infos button
 - pan camera
 - flipbook node

 */

Flooid::Flooid()
: m_graph{}
, m_ui(m_graph)
{
}

void Flooid::Init()
{
    m_renderer.Init();
    m_textureProvider.Init();

    VelocityGen::Init();
    DensityGen::Init();
    Solver::Init();
    Display::Init();


    m_velocityGenNode = new VelocityGen;
    auto velocityIndex = m_graph.AddNode(m_velocityGenNode);

    m_densityGenNode = new DensityGen;
    auto densityGenIndex = m_graph.AddNode(m_densityGenNode);

    m_solverNode = new Solver;
    auto solverIndex = m_graph.AddNode(m_solverNode);
    
    m_displayNode = new Display;
    auto displayIndex = m_graph.AddNode(m_displayNode);
    
    m_graph.AddLink({densityGenIndex, 0, solverIndex, 0});
    m_graph.AddLink({velocityIndex, 0, solverIndex, 1});
    m_graph.AddLink({solverIndex, 1, displayIndex, 0});
    
    m_graph.Layout();
    m_ui.GraphFitAllNodes();
}

void Flooid::Tick(const Parameters& parameters)
{
    if (parameters.lButDown && parameters.enable)
    {
        m_renderer.Input(parameters.dx, parameters.dy);
    }
    m_renderer.Tick();

    if (m_ui.Running())
    {
        m_textureProvider.TickFrame(6);

        m_renderer.Render(m_textureProvider.m_densityTexture);
        
        std::vector<size_t> evaluationOrder = m_graph.ComputeEvaluationOrder();
        m_graph.BuildPlugs();
        
        for (auto evaluationIndex : evaluationOrder)
        {
            GraphNode* node = m_graph.m_nodes[evaluationIndex];
            for (size_t i = 0; i < node->GetInputCount(); i ++)
            {
                auto plug = node->GetPlug(i);
                if (plug.m_node)
                {
                    node->SetInput(i, plug.m_node->GetOutput(plug.m_index));
                }
                else
                {
                    // fallback to persistent texture data
                    // empty input
                    switch(node->GetInputTypes()[i])
                    {
                        case PlugType::Particles:
                            node->SetInput(i, m_textureProvider.m_densityTexture);
                            break;
                        case PlugType::Velocity:
                            node->SetInput(i, m_textureProvider.m_velocityTexture);
                            break;
                        default:
                            assert(0);
                    }
                }
            }
            node->Tick(m_textureProvider);
            node->ReleaseInputs();
        }
    }
    else
    {
        m_renderer.Render(m_textureProvider.m_densityTexture);
    }
}

void Flooid::UI()
{
    m_ui.UI(m_renderer.GetCamera());
}
