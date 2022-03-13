#include "common.h"
#include "bgfx/bgfx.h"
#include "Flooid.h"
#include <stdint.h>
#include "Shaders.h"
#include "GraphNode.h"

/*
 - use advect to reinject density/velocity
 - replace 'm_outputs[0] = velocity;' when ticking node with setoutputtexture, set texture type along
 - set refcount to texture, release decrease it, computing plugs also compute texture refcount
 - bug with colors/io names
 - keep density/velocity each frame
 
 - graph solving with solving list

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
    
    m_densityTexture = m_textureProvider.Acquire();
    m_velocityTexture = m_textureProvider.Acquire();

    Vorticity::Init();
    VelocityGen::Init();
    DensityGen::Init();
    Advection::Init();
    Solver::Init();
    Display::Init();

    m_vorticityNode = new Vorticity;
    auto vorticityIndex = m_graph.AddNode(m_vorticityNode);

    m_velocityGenNode = new VelocityGen;
    auto velocityIndex = m_graph.AddNode(m_velocityGenNode);

    m_densityGenNode = new DensityGen;
    auto densityGenIndex = m_graph.AddNode(m_densityGenNode);

    m_advectDensityNode = new Advection;
    auto advectDensityIndex = m_graph.AddNode(m_advectDensityNode);

    m_advectVelocityNode = new Advection;
    auto advectVelocityIndex = m_graph.AddNode(m_advectVelocityNode);

    m_solverNode = new Solver;
    auto solverIndex = m_graph.AddNode(m_solverNode);
    
    m_displayNode = new Display;
    auto displayIndex = m_graph.AddNode(m_displayNode);
    
    m_graph.AddLink({densityGenIndex, 0, advectDensityIndex, 0});
    m_graph.AddLink({velocityIndex, 0, vorticityIndex, 0});
    m_graph.AddLink({vorticityIndex, 0, solverIndex, 0});
    m_graph.AddLink({solverIndex, 0, advectVelocityIndex, 0});
    m_graph.AddLink({solverIndex, 0, advectVelocityIndex, 1});
    m_graph.AddLink({solverIndex, 0, advectDensityIndex, 1});
    m_graph.AddLink({advectDensityIndex, 0, displayIndex, 0});
    
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
        m_textureProvider.TickInit(6);
        
        std::vector<size_t> evaluationOrder = m_graph.ComputeEvaluationOrder();
        
        /*
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
                        case PlugType::Density:
                            node->SetInput(i, m_densityTexture);
                            break;
                        case PlugType::Velocity:
                            node->SetInput(i, m_velocityTexture);
                            break;
                        default:
                            assert(0);
                    }
                }
            }
            node->Tick(m_textureProvider);
        }
*/
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
        if (0)
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
    else
    {
        m_renderer.Render(m_densityTexture);
    }
}

void Flooid::UI()
{
    m_ui.UI(m_renderer.GetCamera());
}
