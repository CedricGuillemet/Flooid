#include "GraphNode.h"
#include "TextureProvider.h"
#include "Shaders.h"
#include "imgui.h"
#include "GraphEditorDelegate.h"
#include "UIGizmos.h"
#include <algorithm>

const int TEX_SIZE = 256;

void Vorticity::Init()
{
    m_vorticityCSProgram = App::LoadProgram("Vorticity_cs", nullptr);
    m_vorticityForceCSProgram = App::LoadProgram("VorticityForce_cs", nullptr);
    m_texVelocityUniform = bgfx::createUniform("s_texVelocity", bgfx::UniformType::Sampler);
    m_texVorticityUniform = bgfx::createUniform("s_texVorticity", bgfx::UniformType::Sampler);
    m_curlUniform = bgfx::createUniform("curl", bgfx::UniformType::Vec4);
    m_epsilonUniform = bgfx::createUniform("epsilon", bgfx::UniformType::Vec4);

    GraphEditorDelegate::mTemplateFunctions.push_back(GetTemplate);
    _nodeType = GraphNode::_runtimeType++;
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

    GraphEditorDelegate::mTemplateFunctions.push_back(GetTemplate);
    _nodeType = GraphNode::_runtimeType++;
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
    uiGizmos.Edit(&m_position, &m_radius);
    Imm::matrix sphereMatrix;
    sphereMatrix.translationScale({ m_position.x, m_position.y, m_position.z }, {m_radius, m_radius, m_radius});
    uiGizmos.AddSphere(sphereMatrix);
    return changed;
}

// -----------------------------------------------------------------------------------------------------

void VelocityGen::Init()
{
    m_velocityGenCSProgram = App::LoadProgram("VelocityGen_cs", nullptr);
    m_positionUniform = bgfx::createUniform("position", bgfx::UniformType::Vec4);
    m_directionUniform = bgfx::createUniform("direction", bgfx::UniformType::Vec4);

    GraphEditorDelegate::mTemplateFunctions.push_back(GetTemplate);
    _nodeType = GraphNode::_runtimeType++;
}

void VelocityGen::Tick(TextureProvider& textureProvider)
{
    float position[4] = { m_position.x, m_position.y, m_position.z, m_radius };
    bgfx::setUniform(m_positionUniform, position);

    Imm::matrix matrix;
    Imm::vec3 scale{m_radius, m_radius, m_radius};
    ImGuizmo::RecomposeMatrixFromComponents(&m_position.x, &m_orientation.x, &scale.x, matrix.m16);
    float direction[4] = { matrix.dir.x, matrix.dir.y, matrix.dir.z, 0.f };
    bgfx::setUniform(m_directionUniform, direction);

    auto velocity = m_inputs[0];
    bgfx::setImage(0, velocity->GetTexture(), 0, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), m_velocityGenCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    m_outputs[0] = velocity;
}

bool VelocityGen::UI(UIGizmos& uiGizmos)
{
    bool changed = ImGui::InputFloat("Radius", &m_radius);
    uiGizmos.Edit(&m_position, &m_orientation, &m_radius);
    Imm::matrix sphereMatrix;
    sphereMatrix.translationScale({ m_position.x, m_position.y, m_position.z }, { m_radius, m_radius, m_radius });
    uiGizmos.AddSphere(sphereMatrix);
    return changed;
}

// -----------------------------------------------------------------------------------------------------

void Advection::Init()
{
    m_advectCSProgram = App::LoadProgram("Advect_cs", nullptr);
    m_advectionUniform = bgfx::createUniform("advection", bgfx::UniformType::Vec4);
    m_texVelocityUniform = bgfx::createUniform("s_texVelocity", bgfx::UniformType::Sampler);
    m_texAdvectUniform = bgfx::createUniform("s_texAdvect", bgfx::UniformType::Sampler);

    GraphEditorDelegate::mTemplateFunctions.push_back(GetTemplate);
    _nodeType = GraphNode::_runtimeType++;
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

    GraphEditorDelegate::mTemplateFunctions.push_back(GetTemplate);
    _nodeType = GraphNode::_runtimeType++;
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

void Display::Init()
{
    GraphEditorDelegate::mTemplateFunctions.push_back(GetTemplate);
    _nodeType = GraphNode::_runtimeType++;
}

void Display::Tick(TextureProvider& textureProvider)
{
    m_outputs[0] = m_inputs[0];
}

bool Display::UI(UIGizmos& uiGizmos)
{
    uiGizmos.Edit(&m_lightPosition);
    return false;
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

void Graph::RecurseLayout(std::vector<NodePosition>& positions,
    std::map<int, int>& stacks,
    size_t currentIndex,
    int currentLayer)
{
    const auto& nodes = m_nodes;
    const auto& links = m_links;

    if (positions[currentIndex].mLayer == -1)
    {
        positions[currentIndex].mLayer = currentLayer;
        int layer = positions[currentIndex].mLayer = currentLayer;
        if (stacks.find(layer) != stacks.end())
        {
            stacks[layer]++;
        }
        else
        {
            stacks[layer] = 0;
        }
        positions[currentIndex].mStackIndex = stacks[currentLayer];
    }
    else
    {
        // already hooked node
        if (currentLayer > positions[currentIndex].mLayer)
        {
            // remove stack at current pos
            int currentStack = positions[currentIndex].mStackIndex;
            for (auto& pos : positions)
            {
                if (pos.mLayer == positions[currentIndex].mLayer && pos.mStackIndex > currentStack)
                {
                    pos.mStackIndex--;
                    stacks[pos.mLayer]--;
                }
            }
            // apply new one
            int layer = positions[currentIndex].mLayer = currentLayer;
            if (stacks.find(layer) != stacks.end())
                stacks[layer]++;
            else
                stacks[layer] = 0;
            positions[currentIndex].mStackIndex = stacks[currentLayer];
        }
    }

    size_t InputsCount = nodes[currentIndex]->GetInputCount();
    //gMetaNodes[nodes[currentIndex].mNodeType].mInputs.size();
    std::vector<int> inputNodes(InputsCount, -1);
    for (auto& link : links)
    {
        if (link.m_outputNodeIndex != currentIndex)
        {
            continue;
        }
        inputNodes[link.m_outputSlotIndex] = link.m_inputNodeIndex;
    }
    for (auto inputNode : inputNodes)
    {
        if (inputNode == -1)
        {
            continue;
        }
        RecurseLayout(positions, stacks, inputNode, currentLayer + 1);
    }
}

void Graph::Layout(const std::vector<size_t>& orderList)
{
    if (m_nodes.empty())
    {
        return;
    }
    // get stack/layer pos
    std::vector<NodePosition> nodePositions(m_nodes.size(), { -1, -1, -1 });
    std::map<int, int> stacks;
    ImRect sourceRect, destRect;

    std::vector<ImVec2> nodePos(m_nodes.size());

    // compute source bounds
    for (unsigned int i = 0; i < m_nodes.size(); i++)
    {
        const auto& node = *m_nodes[i];
        sourceRect.Add(ImRect({node.m_x, node.m_y}, {node.m_x + 100, node.m_y + 100}));
    }

    for (unsigned int i = 0; i < m_nodes.size(); i++)
    {
        size_t nodeIndex = orderList[m_nodes.size() - i - 1];
        RecurseLayout(nodePositions, stacks, nodeIndex, 0);
    }

    // set corresponding node index in nodePosition
    for (unsigned int i = 0; i < m_nodes.size(); i++)
    {
        int nodeIndex = int(orderList[i]);
        auto& layout = nodePositions[nodeIndex];
        layout.mNodeIndex = nodeIndex;
    }

    // sort nodePositions
    std::sort(nodePositions.begin(), nodePositions.end());

    // set x,y position from layer/stack
    float currentStackHeight = 0.f;
    int currentLayerIndex = -1;
    for (unsigned int i = 0; i < nodePositions.size(); i++)
    {
        auto& layout = nodePositions[i];
        if (currentLayerIndex != layout.mLayer)
        {
            currentLayerIndex = layout.mLayer;
            currentStackHeight = 0.f;
        }
        size_t nodeIndex = layout.mNodeIndex;
        const auto& node = m_nodes[nodeIndex];
        float height = 120;//float(gMetaNodes[node.mNodeType].mHeight);
        nodePos[nodeIndex] = ImVec2(-layout.mLayer * 280.f, currentStackHeight);
        currentStackHeight += height + 40.f;
    }

    // new bounds
    for (unsigned int i = 0; i < m_nodes.size(); i++)
    {
        ImVec2 newPos = nodePos[i];
        // todo: support height more closely with metanodes
        destRect.Add(ImRect(newPos, {newPos.x + 100, newPos.y + 100}));
    }

    // move all nodes
    ImVec2 offset = sourceRect.GetCenter();
    ImVec2 destCenter = destRect.GetCenter();
    offset.x -= destCenter.x;
    offset.y -= destCenter.y;
    for (auto& pos : nodePos)
    {
        pos.x += offset.x;
        pos.y += offset.y;
    }
    for (auto i = 0; i < m_nodes.size(); i++)
    {
        //SetNodePosition(i, nodePos[i]);
        m_nodes[i]->m_x = nodePos[i].x;
        m_nodes[i]->m_y = nodePos[i].y;
    }
}

size_t Graph::PickBestNode(const std::vector<NodeOrder>& orders)
{
    for (auto& order : orders)
    {
        if (order.mNodePriority == 0)
        {
            return order.mNodeIndex;
        }
    }
    // issue!
    assert(0);
    return -1;
}

void Graph::RecurseSetPriority(std::vector<NodeOrder>& orders,
                        const std::vector<Link>& links,
                        size_t currentIndex,
                        size_t currentPriority,
                        size_t& undeterminedNodeCount)
{
    if (!orders[currentIndex].mNodePriority)
    {
        undeterminedNodeCount--;
    }

    orders[currentIndex].mNodePriority = std::max(orders[currentIndex].mNodePriority, currentPriority + 1);
    for (auto& link : links)
    {
        if (link.m_outputNodeIndex == currentIndex)
        {
            RecurseSetPriority(orders, links, link.m_inputNodeIndex, currentPriority + 1, undeterminedNodeCount);
        }
    }
}

std::vector<Graph::NodeOrder> Graph::ComputeEvaluationOrders(const std::vector<Link>& links, size_t nodeCount)
{
    std::vector<NodeOrder> orders(nodeCount);
    for (size_t i = 0; i < nodeCount; i++)
    {
        orders[i].mNodeIndex = i;
        orders[i].mNodePriority = 0;
    }
    size_t undeterminedNodeCount = nodeCount;
    while (undeterminedNodeCount)
    {
        size_t currentIndex = PickBestNode(orders);
        RecurseSetPriority(orders, links, currentIndex, orders[currentIndex].mNodePriority, undeterminedNodeCount);
    };
    return orders;
}

std::vector<size_t> Graph::ComputeEvaluationOrder()
{
    std::vector<size_t> orderList;

    auto orders = ComputeEvaluationOrders(m_links, m_nodes.size());
    std::sort(orders.begin(), orders.end());
    orderList.resize(orders.size());
    for (size_t i = 0; i < orders.size(); i++)
    {
        orderList[i] = orders[i].mNodeIndex;
    }
    return orderList;
}
