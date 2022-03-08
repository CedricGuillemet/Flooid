#include "TextureProvider.h"
#include "Renderer.h"
#include "GraphEditorDelegate.h"
#include "FlooidUI.h"

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

    Texture* m_densityTexture{};
    Texture* m_velocityTexture{};
    
    TextureProvider m_textureProvider;
    Renderer m_renderer;

    Vorticity* m_vorticityNode{};
    VelocityGen* m_velocityGenNode{};
    DensityGen* m_densityGenNode{};
    Advection* m_advectDensityNode{};
    Advection* m_advectVelocityNode{};
    Solver* m_solverNode{};
    Graph m_graph;
    
protected:
    FlooidUI m_ui;
};
