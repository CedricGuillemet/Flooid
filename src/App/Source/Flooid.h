#include "TextureProvider.h"
#include "Renderer.h"
#include "GraphEditorDelegate.h"

class Flooid
{
public:
    Flooid();

    struct Parameters
    {
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
    
    TextureProvider m_textureProvider;
    Renderer m_renderer;

    Vorticity* m_vorticityNode;
    VelocityGen* m_velocityGenNode;
    DensityGen* m_densityGenNode;
    Advection* m_advectDensityNode;
    Advection* m_advectVelocityNode;
    Solver* m_solverNode;
    Graph m_graph;
    
    GraphEditor::Options m_graphEditorOptions;
    GraphEditorDelegate m_graphEditorDelegate;
    GraphEditor::ViewState m_graphEditorViewState{{0.0f, 0.0f}, { 0.5f },{ 0.5f }};
    GraphEditor::FitOnScreen m_graphEditorFit = GraphEditor::Fit_None;
protected:
    void CheckUsingUI();
    bool m_usingGUI{false};
};
