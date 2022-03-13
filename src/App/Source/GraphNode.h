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
static inline uint16_t _nodeType = GraphNode::_runtimeType++; \
public:\
uint16_t GetRuntimeType() const { return _nodeType; } \
size_t GetInputCount() const { return GraphNodeIO::GetInputCount(); } \
size_t GetOutputCount() const { return GraphNodeIO::GetInputCount(); } \
void ClearPlugs() { GraphNodeIO::ClearPlugs(); } \
void SetPlug(uint8_t slotIndex, const Plug plug) { GraphNodeIO::SetPlug(slotIndex, plug); } \
const Plug GetPlug(uint8_t slotIndex) const { return GraphNodeIO::GetPlug(slotIndex); } \
void SetInput(unsigned int inputIndex, Texture* texture) { GraphNodeIO::SetInput(inputIndex, texture); } \
Texture* GetOutput(unsigned int outputIndex) { return GraphNodeIO::GetOutput(outputIndex); }


struct PlugType
{
    enum Enum
    {
        Velocity,
        Density,
        Image,
        Any,
        Count,
    };
};

struct Plug
{
    GraphNode* m_node;
    uint8_t m_index;
};

template<unsigned int inputCount, unsigned int outputCount> class GraphNodeIO
{
public:
    GraphNodeIO()
    {
        m_inputs = new Texture* [inputCount];
        m_outputs = new Texture* [outputCount];
        m_plugs = new Plug [inputCount];
    }
    
    ~GraphNodeIO()
    {
        delete [] m_inputs;
        delete [] m_outputs;
        delete [] m_plugs;
    }
    
    void SetInput(unsigned int inputIndex, Texture* texture)
    {
        assert(inputIndex < inputCount);
        m_inputs[inputIndex] = texture;
    }
    Texture* GetOutput(unsigned int outputIndex)
    {
        assert(outputIndex < outputCount);
        return m_outputs[outputIndex];
    }
    
    size_t GetInputCount() const { return inputCount; }
    size_t GetOutputCount() const { return outputCount; }
    
    void ClearPlugs()
    {
        for (size_t i = 0; i < inputCount; i++)
        {
            m_plugs[i] = {nullptr, 0xFF};
        }
    }
    
    void SetPlug(uint8_t slotIndex, const Plug plug)//GraphNode* inputNode, uint8_t inputSlotIndex)
    {
        assert(slotIndex < inputCount);
        m_plugs[slotIndex] = plug;
    }
    
    const Plug GetPlug(uint8_t slotIndex) const
    {
        return m_plugs[slotIndex];
    }
    
protected:
    Plug* m_plugs;
    Texture** m_inputs;
    Texture** m_outputs;
};

class GraphNode
{
public:
    GraphNode()
    : m_x(0.f)
    , m_y(0.f)
    , m_selected(false)
    {}
    virtual const char* GetName() const = 0;
    virtual void Tick(TextureProvider& textureProvider) = 0;
    virtual bool UI(UIGizmos& uiGizmos) = 0;
    virtual uint16_t GetRuntimeType() const = 0;
    virtual size_t GetInputCount() const = 0;
    virtual size_t GetOutputCount() const = 0;
    virtual void ClearPlugs() = 0;
    virtual void SetPlug(uint8_t slotIndex, const Plug plug) = 0;
    virtual const Plug GetPlug(uint8_t slotIndex) const = 0;
    virtual const PlugType::Enum* const GetOutputTypes() const = 0;
    virtual const PlugType::Enum* const GetInputTypes() const = 0;
    virtual void SetInput(unsigned int inputIndex, Texture* texture) = 0;
    virtual Texture* GetOutput(unsigned int outputIndex) = 0;

    //protected:
    
    static uint32_t GetPlugColor(PlugType::Enum plugType)
    {
        switch(plugType)
        {
            case PlugType::Velocity:
                return IM_COL32(200, 100, 50, 255);
            case PlugType::Density:
                return IM_COL32(50, 100, 200, 255);
            case PlugType::Image:
                return IM_COL32(150, 200, 150, 255);
            case PlugType::Any:
                return IM_COL32(50, 50, 200, 255);
        }
        return IM_COL32(255, 0, 255, 255);
    }
    
    float m_x, m_y;
    bool m_selected;
    
    static inline uint16_t _runtimeType{};
};

class Vorticity : public GraphNode, public GraphNodeIO<1,1>
{
public:
    Vorticity()
    : m_curl(2.f)
    , m_epsilon(0.0002f)
    {}
    const char* GetName() const { return "Vorticity"; }
    
    static void Init();
    void Tick(TextureProvider& textureProvider);
    bool UI(UIGizmos& uiGizmos);
    
    const PlugType::Enum* const GetOutputTypes() const { return Imm::Array{PlugType::Velocity}; }
    const PlugType::Enum* const GetInputTypes() const { return Imm::Array{PlugType::Velocity}; }
    
    static GraphEditor::Template GetTemplate()
    {
        return {
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            1,
            Imm::Array{"Velocity"},
            Imm::Array{ GetPlugColor(PlugType::Velocity) },
            1,
            Imm::Array{"Velocity"},
            Imm::Array{ GetPlugColor(PlugType::Velocity) }
        };
    }
private:
    static inline bgfx::ProgramHandle m_vorticityCSProgram;
    static inline bgfx::ProgramHandle m_vorticityForceCSProgram;
    static inline bgfx::UniformHandle m_texVorticityUniform;
    static inline bgfx::UniformHandle m_texVelocityUniform;
    static inline bgfx::UniformHandle m_curlUniform;
    static inline bgfx::UniformHandle m_epsilonUniform;
    
    float m_curl;
    float m_epsilon;
    
    __NODE_TYPE
};

class DensityGen : public GraphNode, public GraphNodeIO<1, 1>
{
public:
    DensityGen()
    : m_position{0.5f, 0.05f, 0.f}
    , m_radius(0.1f)
    {}
    const char* GetName() const { return "Density Gen"; }
    
    static void Init();
    void Tick(TextureProvider& textureProvider);
    bool UI(UIGizmos& uiGizmos);
    const PlugType::Enum* const GetOutputTypes() const { return Imm::Array{PlugType::Density}; }
    const PlugType::Enum* const GetInputTypes() const { return Imm::Array{PlugType::Density}; }

    static GraphEditor::Template GetTemplate()
    {
        return {
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            1,
            Imm::Array{"Density"},
            Imm::Array{ GetPlugColor(PlugType::Density) },
            1,
            Imm::Array{"Density"},
            Imm::Array{ GetPlugColor(PlugType::Density) }
        };
    }
private:
    static inline bgfx::ProgramHandle m_densityGenCSProgram;
    static inline bgfx::UniformHandle m_positionUniform;
    
    Imm::vec3 m_position;
    float m_radius;
    
    __NODE_TYPE
};

class VelocityGen : public GraphNode, public GraphNodeIO<1, 1>
{
public:
    VelocityGen()
    : m_position{0.5f, 0.05f, 0.f}
    , m_orientation{-Imm::PI * 0.5f, 0.f, 0.f}
    , m_radius(0.1f)
    {
        
    }
    const char* GetName() const { return "Velocity Gen"; }
    
    static void Init();
    void Tick(TextureProvider& textureProvider);
    bool UI(UIGizmos& uiGizmos);
    const PlugType::Enum* const GetOutputTypes() const { return Imm::Array{PlugType::Velocity}; }
    const PlugType::Enum* const GetInputTypes() const { return Imm::Array{PlugType::Velocity}; }

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


class Advection : public GraphNode, public GraphNodeIO<2, 1>
{
public:
    Advection()
    : m_timeScale(1.f)
    , m_dissipation(0.997f)
    {
        
    }
    const char* GetName() const { return "Advection"; }
    
    static void Init();
    void Tick(TextureProvider& textureProvider);
    bool UI(UIGizmos& uiGizmos);
    const PlugType::Enum* const GetOutputTypes() const { return Imm::Array{PlugType::Velocity, PlugType::Any}; }
    const PlugType::Enum* const GetInputTypes() const { return Imm::Array{PlugType::Any}; }

    static GraphEditor::Template GetTemplate()
    {
        return {
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            2,
            Imm::Array{"Velocity", "Source"},
            Imm::Array{ GetPlugColor(PlugType::Velocity), GetPlugColor(PlugType::Any)},
            1,
            Imm::Array{"Advected"},
            Imm::Array{ GetPlugColor(PlugType::Any)}
        };
    }
private:
    static inline bgfx::ProgramHandle m_advectCSProgram;
    static inline bgfx::UniformHandle m_advectionUniform;
    static inline bgfx::UniformHandle m_texVelocityUniform;
    static inline bgfx::UniformHandle m_texAdvectUniform;
    float m_timeScale;
    float m_dissipation;
    
    __NODE_TYPE
};

class Solver : public GraphNode, public GraphNodeIO<1, 1>
{
public:
    Solver()
    : m_alpha(-1.f)
    , m_beta(4)
    , m_iterationCount(50)
    {
        
    }
    const char* GetName() const { return "Solver"; }
    
    static void Init();
    void Tick(TextureProvider& textureProvider);
    bool UI(UIGizmos& uiGizmos);
    const PlugType::Enum* const GetOutputTypes() const { return Imm::Array{PlugType::Velocity}; }
    const PlugType::Enum* const GetInputTypes() const { return Imm::Array{PlugType::Velocity}; }

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
    static inline bgfx::ProgramHandle m_divergenceCSProgram;
    static inline bgfx::ProgramHandle m_gradientCSProgram;
    static inline bgfx::ProgramHandle m_jacobiCSProgram;
    static inline bgfx::UniformHandle m_jacobiParametersUniform;
    static inline bgfx::UniformHandle m_texVelocityUniform;
    static inline bgfx::UniformHandle m_texJacoviUniform;
    static inline bgfx::UniformHandle m_texDivergenceUniform;
    static inline bgfx::UniformHandle m_texColorUniform;
    static inline bgfx::UniformHandle m_texPressureUniform;
    float m_alpha;
    float m_beta;
    int m_iterationCount;
    
    __NODE_TYPE
};

class Display : public GraphNode, public GraphNodeIO<1, 1>
{
public:
    Display()
    : m_lightPosition{0.15f, 1.2f, 1.f}
    {
    }
    const char* GetName() const { return "Display"; }
    
    static void Init();
    void Tick(TextureProvider& textureProvider);
    bool UI(UIGizmos& uiGizmos);
    const PlugType::Enum* const GetOutputTypes() const { return Imm::Array{PlugType::Density}; }
    const PlugType::Enum* const GetInputTypes() const { return Imm::Array{PlugType::Image}; }

    static GraphEditor::Template GetTemplate()
    {
        return {
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            1,
            Imm::Array{"Density"},
            Imm::Array{ GetPlugColor(PlugType::Density)},
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
        }
    }
};
