#include "TextureProvider.h"
#include "Renderer.h"
#include "GraphEditorDelegate.h"

class Flooid
{
public:
    Flooid();

    struct Parameters
    {
        int m_iterationCount;
        int m_display;
        float x, y;
        float dx, dy;
        bool lButDown, rButDown;
    };

    void Init();
    void SetDisplaySize(uint16_t width, uint16_t height) { m_renderer.SetDisplaySize(width, height); }
    void Tick(const Parameters& parameters);
    
    // return true if over or using UI
    bool UI();

    Texture* m_densityTexture;
    Texture* m_velocityTexture;
    
    bgfx::UniformHandle m_jacobiParametersUniform;
    bgfx::UniformHandle m_advectionUniform;

    bgfx::UniformHandle m_texVelocityUniform;
    bgfx::UniformHandle m_texAdvectUniform;
    bgfx::UniformHandle m_texColorUniform;
    bgfx::UniformHandle m_texPressureUniform;
    bgfx::UniformHandle m_texVorticityUniform;

    bgfx::UniformHandle m_texJacoviUniform;
    bgfx::UniformHandle m_texDivergenceUniform;

    bgfx::ProgramHandle m_divergenceCSProgram;
    bgfx::ProgramHandle m_gradientCSProgram;
    bgfx::ProgramHandle m_jacobiCSProgram;
    bgfx::ProgramHandle m_advectCSProgram;

    
    TextureProvider m_textureProvider;
    Renderer m_renderer;
    
    
    Vorticity* m_vorticityNode;
    VelocityGen* m_velocityGenNode;
    DensityGen* m_densityGenNode;
    Graph m_graph;
    
    GraphEditor::Options m_graphEditorOptions;
    GraphEditorDelegate m_graphEditorDelegate;
    GraphEditor::ViewState m_graphEditorViewState{{0.0f, 0.0f}, { 0.5f },{ 0.5f }};
    GraphEditor::FitOnScreen m_graphEditorFit = GraphEditor::Fit_None;
protected:
    void CheckUsingUI();
    bool m_usingGUI{false};
};
