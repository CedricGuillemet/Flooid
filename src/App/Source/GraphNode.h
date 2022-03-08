#pragma once
#include <string>
#include <bgfx/bgfx.h>
#include "Immath.h"
#include "GraphEditor.h"
class TextureProvider;
struct Texture;


template<unsigned int inputCount, unsigned int outputCount> class GraphNodeIO
{
public:
    GraphNodeIO() {}
    
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
protected:
    Texture* m_inputs[inputCount];
    Texture* m_outputs[outputCount];
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
    virtual bool Edit() = 0;
//protected:
    float m_x, m_y;
    bool m_selected;
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
    bool Edit();
    static GraphEditor::Template GetTemplate()
    {
        return {
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            1,
            Imm::Array{"Velocity"},
            Imm::Array{ IM_COL32(200,200,200,255)},
            1,
            Imm::Array{"Velocity"},
            Imm::Array{ IM_COL32(200,200,200,255)}
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
};

class Graph
{
public:
    Graph() {}
    
    void AddNode(GraphNode* node) { m_nodes.push_back(node); }
    std::vector<GraphNode*>& GetNodes() { return m_nodes; }
    std::vector<GraphNode*> m_nodes;
};
