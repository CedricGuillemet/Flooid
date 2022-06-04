#include "common.h"
#include "bgfx/bgfx.h"
#include "Flooid.h"
#include <stdint.h>
#include "Shaders.h"
#include "GraphNode.h"

/*
 - clear allocated pages
 - debug display with log scale/colors
 - commonfluid.sh include
 - debug UI with more mips
 - factor code downsample/residual
 - more levels
 - fix bugs
 - modularity : resolution virtual size, page count
 
 
 - The sign of the Residual seems incorrect and inconsistent with the Jacobi pass
 - The downscaled residual doesn't seem to be scaled correctly
 - It's not handling the alignment correctly on the downscale. Upscale seems fine, but not quite sure.
 - The boundary is not consistent between the fine and coarse grids, because the coarse tiles are the same size (16x16) as the fine tiles, and the space covered by the coarse grid is a larger than the fine grid.
 - Then there's whatever is the issue with dilated tiles, not sure what's going on there.

 Based on that I expect it to actually only really have much of an effect on the fine grid iterations (there's 8 in total)
 shd:
 
 - light color, absorbtion
 - more gen prims + noise params
 - move rendering in node, separate solver node
 - smoke rendering
 
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
//: m_graph{}
//, m_ui(m_graph)
{
}

void Flooid::Init()
{
    m_renderer.Init();
    m_textureProvider.Init();
#if 0
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
    /*

    m_densityGenNode = new DensityGen;
    auto densityGenIndex = m_graph.AddNode(m_densityGenNode);

    m_displayNode = new Display;
    auto displayIndex = m_graph.AddNode(m_displayNode);

    m_graph.AddLink({ densityGenIndex, 0, displayIndex, 0 });
*/
    m_graph.Layout();
    m_ui.GraphFitAllNodes();
#endif
    mGPU.Init(m_textureProvider);
}

void Flooid::Tick(const Parameters& parameters)
{
    if (parameters.lButDown && parameters.enable)
    {
        m_renderer.Input(parameters.dx, parameters.dy);
    }
    m_renderer.Tick();

    /*if (m_ui.Running())
    {
        m_textureProvider.TickFrame(6);

        m_renderer.Render(m_textureProvider, m_textureProvider.m_densityTexture, m_displayNode);
        
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
    */

    m_textureProvider.TickFrame(6);
    //m_renderer.Render(m_textureProvider, mGPU.mWorldToPages, mGPU.mDensityPages, nullptr);
    m_renderer.Render(m_textureProvider, mGPU.GetDisplayPageIndirection(), mGPU.GetDisplayPages(), mGPU.mWorldToPageTags, nullptr);
    mGPU.Tick(m_textureProvider);
    
    
}

void Flooid::UI()
{
    //m_ui.UI(m_renderer.GetCamera());

    ImGui::SetNextWindowPos(ImVec2(0.0f, 100.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(200, 600), ImGuiCond_FirstUseEver);

    ImGui::Begin("Parameters", NULL, 0);
    mGPU.UI();
    ImGui::End();
}
