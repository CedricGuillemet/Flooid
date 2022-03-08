#pragma once
#include <string>
#include <bgfx/bgfx.h>
#include "Immath.h"
#include "GraphEditor.h"
class TextureProvider;
struct Texture;


#define __NODE_TYPE \
private:\
static inline uint16_t _nodeType = GraphNode::_runtimeType++; \
public:\
uint16_t GetRuntimeType() const { return _nodeType; }

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
    virtual bool UI() = 0;
    virtual uint16_t GetRuntimeType() const = 0;
//protected:
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
    bool UI();
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

    __NODE_TYPE
};

class DensityGen : public GraphNode, public GraphNodeIO<1, 1>
{
public:
    DensityGen()
        : m_position(0.5f, 0.95f, 0.f)
        , m_radius(0.1f)
    {

    }
    const char* GetName() const { return "Density Gen"; }

    static void Init();
    void Tick(TextureProvider& textureProvider);
    bool UI();
    static GraphEditor::Template GetTemplate()
    {
        return {
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            1,
            Imm::Array{"Density"},
            Imm::Array{ IM_COL32(200,200,200,255)},
            1,
            Imm::Array{"Density"},
            Imm::Array{ IM_COL32(200,200,200,255)}
        };
    }
private:
    static inline bgfx::ProgramHandle m_densityGenCSProgram;
    static inline bgfx::UniformHandle m_positionUniform;
    
    Imm::vec4 m_position;
    float m_radius;

    __NODE_TYPE
};

class VelocityGen : public GraphNode, public GraphNodeIO<1, 1>
{
public:
    VelocityGen()
        : m_position(0.5f, 0.95f, 0.f)
        , m_direction(0.f, -0.01f, 0.f)
        , m_radius(0.1f)
    {

    }
    const char* GetName() const { return "Velocity Gen"; }

    static void Init();
    void Tick(TextureProvider& textureProvider);
    bool UI();
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
    static inline bgfx::ProgramHandle m_velocityGenCSProgram;
    static inline bgfx::UniformHandle m_positionUniform;
    static inline bgfx::UniformHandle m_directionUniform;
    Imm::vec4 m_position;
    Imm::vec4 m_direction;
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
    bool UI();
    static GraphEditor::Template GetTemplate()
    {
        return {
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            2,
            Imm::Array{"Velocity", "Source"},
            Imm::Array{ IM_COL32(200,200,200,255)},
            1,
            Imm::Array{"Advected"},
            Imm::Array{ IM_COL32(200,200,200,255)}
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
    bool UI();
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

class Graph
{
public:
    Graph() {}
    
    void AddNode(GraphNode* node) { m_nodes.push_back(node); }
    std::vector<GraphNode*>& GetNodes() { return m_nodes; }
    std::vector<GraphNode*> m_nodes;
};
