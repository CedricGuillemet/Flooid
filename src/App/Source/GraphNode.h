#pragma once
#include <string>
#include <map>
#include <bgfx/bgfx.h>
#include "Immath.h"
#include "GraphEditor.h"

class TextureProvider;
struct Texture;
class UIGizmos;
class GraphNode;

#define __NODE_TYPE \
private:\
static inline uint16_t _nodeType{}; \
public:\
uint16_t GetRuntimeType() const { return _nodeType; } \
static inline std::vector<PlugType::Enum> m_inputTypes; \
static inline std::vector<PlugType::Enum> m_outputTypes; \
const std::vector<PlugType::Enum>& GetOutputTypes() const { return m_outputTypes; } \
const std::vector<PlugType::Enum>& GetInputTypes() const { return m_inputTypes; }

struct PlugType
{
    enum Enum
    {
        Velocity,
        Particles,
        Image,
        Any,
        
        Invalid,
        Count,
    };
};

struct Plug
{
    GraphNode* m_node;
    uint8_t m_index;
};

class GraphNode
{
public:
    GraphNode(size_t inputCount, size_t outputCount)
    : m_x(0.f)
    , m_y(0.f)
    , m_selected(false)
    {
        m_inputs.resize(inputCount);
        m_outputs.resize(outputCount);
        m_outputUseCount.resize(outputCount);
        m_plugs.resize(inputCount);
    }
    
    virtual const char* GetName() const = 0;
    virtual void Tick(TextureProvider& textureProvider) = 0;
    virtual bool UI(UIGizmos& uiGizmos) = 0;
    virtual uint16_t GetRuntimeType() const = 0;
    virtual size_t GetInputCount() const { return m_inputs.size(); }
    virtual size_t GetOutputCount() const { return m_outputs.size(); }
    virtual const std::vector<PlugType::Enum>& GetOutputTypes() const = 0;
    virtual const std::vector<PlugType::Enum>& GetInputTypes() const = 0;
    virtual void SetInput(unsigned int slotIndex, Texture* texture) { m_inputs[slotIndex] = texture; }
    virtual Texture* GetOutput(unsigned int slotIndex) { return m_outputs[slotIndex]; }
    virtual Texture* GetInput(uint8_t slotIndex) { return m_inputs[slotIndex]; }

    void SetOutput(unsigned int outputIndex, Texture* texture);
    //protected:
    
    void ReleaseInputs()
    {
        
    }
    
    void ClearPlugs()
    {
        for (size_t i = 0; i < m_plugs.size(); i++)
        {
            m_plugs[i] = {nullptr, 0xFF};
        }
        for (size_t i = 0; i < m_outputUseCount.size(); i++)
        {
            m_outputUseCount[i] = 0;
        }
    }
    
    void IncreaseOuputCount(int slotIndex)
    {
        m_outputUseCount[slotIndex]++;
    }
    
    void SetPlug(uint8_t slotIndex, const Plug plug)//GraphNode* inputNode, uint8_t inputSlotIndex)
    {
        m_plugs[slotIndex] = plug;
    }
    
    const Plug GetPlug(uint8_t slotIndex) const
    {
        return m_plugs[slotIndex];
    }
    static uint32_t GetPlugColor(PlugType::Enum plugType)
    {
        switch(plugType)
        {
            case PlugType::Velocity:
                return IM_COL32(200, 100, 50, 255);
            case PlugType::Particles:
                return IM_COL32(0, 0, 200, 255);
            case PlugType::Image:
                return IM_COL32(0, 200, 0, 255);
            case PlugType::Any:
                return IM_COL32(200, 200, 200, 255);
            default:
                return IM_COL32(255, 0, 255, 255);
        }
        return IM_COL32(255, 0, 255, 255);
    }
    
    float m_x, m_y;
    bool m_selected;
    
    static inline uint16_t _runtimeType{};
    
    
    std::vector<Plug> m_plugs;
    std::vector<Texture*> m_inputs;
    std::vector<Texture*> m_outputs;
    std::vector<uint16_t> m_outputUseCount;
};

class DensityGen : public GraphNode
{
public:
    DensityGen()
    : GraphNode(1, 1)
    {
        m_matrix.translationScale({0.5f, 0.5f, 0.5f}, 0.45f);
    }
    const char* GetName() const { return "Density Gen"; }
    
    static void Init();
    void Tick(TextureProvider& textureProvider);
    bool UI(UIGizmos& uiGizmos);

    static GraphEditor::Template GetTemplate()
    {
        return {
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            1,
            Imm::Array{"Density"},
            Imm::Array{ GetPlugColor(PlugType::Particles) },
            1,
            Imm::Array{"Density"},
            Imm::Array{ GetPlugColor(PlugType::Particles) }
        };
    }
private:
    static inline bgfx::ProgramHandle m_densityGenCSProgram;
    static inline bgfx::UniformHandle m_invWorldMatrixUniform;
    
    Imm::matrix m_matrix;

    __NODE_TYPE
};

class VelocityGen : public GraphNode
{
public:
    VelocityGen()
    : GraphNode(1, 1)
    , m_position{0.5f, 0.05f, 0.f}
    , m_orientation{-Imm::PI * 0.5f, 0.f, 0.f}
    , m_radius(0.1f)
    {
        
    }
    const char* GetName() const { return "Velocity Gen"; }
    
    static void Init();
    void Tick(TextureProvider& textureProvider);
    bool UI(UIGizmos& uiGizmos);

    static GraphEditor::Template GetTemplate()
    {
        return {
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            1,
            Imm::Array{"Velocity"},
            Imm::Array{ GetPlugColor(PlugType::Velocity)},
            1,
            Imm::Array{"Velocity"},
            Imm::Array{ GetPlugColor(PlugType::Velocity)}
        };
    }
private:
    static inline bgfx::ProgramHandle m_velocityGenCSProgram;
    static inline bgfx::UniformHandle m_positionUniform;
    static inline bgfx::UniformHandle m_directionUniform;
    Imm::vec3 m_position;
    Imm::vec3 m_orientation;
    float m_radius;
    
    __NODE_TYPE
};


class Solver : public GraphNode
{
public:
    Solver()
    : GraphNode(2, 2)
    , m_alpha(-1.f)
    , m_beta(4)
    , m_iterationCount(50)
    , m_timeScale(1.f)
    , m_dissipation(0.997f)
    , m_curl(2.f)
    , m_epsilon(0.0002f)
    , m_vorticityEnable(true)
    {
    }
    const char* GetName() const { return "Solver"; }
    
    static void Init();
    void Tick(TextureProvider& textureProvider);
    bool UI(UIGizmos& uiGizmos);

    static GraphEditor::Template GetTemplate()
    {
        return {
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            2,
            Imm::Array{"Particles", "Velocity"},
            Imm::Array{ GetPlugColor(PlugType::Particles), GetPlugColor(PlugType::Velocity)},
            2,
            Imm::Array{"Particles", "Velocity"},
            Imm::Array{ GetPlugColor(PlugType::Particles), GetPlugColor(PlugType::Velocity)}
        };
    }
    
    static inline bgfx::ProgramHandle m_clearCSProgram;
private:

    void Advect(TextureProvider& textureProviderTexture, Texture* source, Texture* velocity, Texture* output);

    static inline bgfx::ProgramHandle m_divergenceCSProgram;
    static inline bgfx::ProgramHandle m_gradientCSProgram;
    static inline bgfx::ProgramHandle m_jacobiCSProgram;
    static inline bgfx::ProgramHandle m_advectCSProgram;
    static inline bgfx::ProgramHandle m_vorticityCSProgram;
    
    static inline bgfx::ProgramHandle m_vorticityForceCSProgram;
    static inline bgfx::UniformHandle m_texVorticityUniform;
    static inline bgfx::UniformHandle m_texVelocityUniform;
    static inline bgfx::UniformHandle m_curlUniform;
    static inline bgfx::UniformHandle m_epsilonUniform;

    
    static inline bgfx::UniformHandle m_jacobiParametersUniform;
    static inline bgfx::UniformHandle m_texJacoviUniform;
    static inline bgfx::UniformHandle m_texDivergenceUniform;
    static inline bgfx::UniformHandle m_texColorUniform;
    static inline bgfx::UniformHandle m_texPressureUniform;
    static inline bgfx::UniformHandle m_advectionUniform;
    static inline bgfx::UniformHandle m_texAdvectUniform;

    float m_timeScale;
    float m_dissipation;
    float m_curl;
    float m_epsilon;

    float m_alpha;
    float m_beta;
    int m_iterationCount;

    bool m_vorticityEnable;
    
    __NODE_TYPE
};

class Display : public GraphNode
{
public:
    Display()
    : GraphNode(1, 1)
    , m_lightPosition{0.15f, 1.2f, 1.f}
    {
    }
    const char* GetName() const { return "Display"; }
    
    static void Init();
    void Tick(TextureProvider& textureProvider);
    bool UI(UIGizmos& uiGizmos);
    
    Imm::vec3 GetLightPosition() const { return m_lightPosition; }

    static GraphEditor::Template GetTemplate()
    {
        return {
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            1,
            Imm::Array{"Particles"},
            Imm::Array{ GetPlugColor(PlugType::Particles)},
            1,
            Imm::Array{"Image"},
            Imm::Array{ GetPlugColor(PlugType::Image)}
        };
    }
private:
    Imm::vec3 m_lightPosition;
    
    __NODE_TYPE
};

class Graph
{
public:
    Graph() {}
    
    struct Link
    {
        uint32_t m_inputNodeIndex;
        uint8_t m_inputSlotIndex;
        uint32_t m_outputNodeIndex;
        uint8_t m_outputSlotIndex;
    };
    
    struct NodePlug
    {
        uint32_t m_nodeIndex;
        uint8_t m_plugIndex;
    };
    
    // return selected node, nullptr is more than 1 selected or none
    GraphNode* GetSelectedNode() const;
    uint32_t AddNode(GraphNode* node) { m_nodes.push_back(node); return static_cast<uint32_t>(m_nodes.size() - 1); }
    
    std::vector<GraphNode*>& GetNodes() { return m_nodes; }
    
    void AddLink(const Link link) { m_links.push_back(link); }
    const std::vector<Link>& GetLinks() const { return m_links; }
    void EraseLink(size_t linkIndex)
    {
        m_links.erase(m_links.begin() + linkIndex);
    }
    
    std::vector<GraphNode*> m_nodes;
    std::vector<Link> m_links;
    
    void UnselectAll()
    {
        for(auto node : m_nodes)
        {
            node->m_selected = false;
        }
    }
    // layout
    struct NodePosition
    {
        int mLayer;
        int mStackIndex;
        int mNodeIndex; // used for sorting
        
        bool operator <(const NodePosition& other) const
        {
            if (mLayer < other.mLayer)
            {
                return true;
            }
            if (mLayer > other.mLayer)
            {
                return false;
            }
            if (mStackIndex<other.mStackIndex)
            {
                return true;
            }
            return false;
        }
    };
    
    void Layout() { Layout(ComputeEvaluationOrder()); }
    void Layout(const std::vector<size_t>& orderList);
    
    void RecurseLayout(std::vector<NodePosition>& positions,
                       std::map<int, int>& stacks,
                       size_t currentIndex, int currentLayer);
    
    struct NodeOrder
    {
        size_t mNodeIndex;
        size_t mNodePriority;
        bool operator<(const NodeOrder& other) const
        {
            return other.mNodePriority < mNodePriority; // reverse order compared to priority value: lower last
        }
    };
    
    
    void RecurseSetPriority(std::vector<NodeOrder>& orders,
                            const std::vector<Link>& links,
                            size_t currentIndex,
                            size_t currentPriority,
                            size_t& undeterminedNodeCount);
    
    std::vector<NodeOrder> ComputeEvaluationOrders(const std::vector<Link>& links, size_t nodeCount);
    size_t PickBestNode(const std::vector<NodeOrder>& orders);
    std::vector<size_t> ComputeEvaluationOrder();
    
    void BuildPlugs()
    {
        for (auto node : m_nodes)
        {
            node->ClearPlugs();
        }
        
        for (auto link : m_links)
        {
            m_nodes[link.m_outputNodeIndex]->SetPlug(link.m_outputSlotIndex, {m_nodes[link.m_inputNodeIndex], link.m_inputSlotIndex});
            //m_nodes[link.m_outputNodeIndex]->IncreaseOuputCount(link.m_outputSlotIndex);
        }
    }
};
