#include "TextureProvider.h"
#include "Renderer.h"
#include "GraphEditorDelegate.h"
#include "FlooidUI.h"
#include "tgpu.h"

class Flooid
{
public:
    Flooid();

    struct Parameters
    {
        float x, y;
        float dx, dy;
        bool lButDown, rButDown;
        bool enable;
    };

    void Init();
    void SetDisplaySize(uint16_t width, uint16_t height) { m_renderer.SetDisplaySize(width, height); }
    void Tick(const Parameters& parameters);
    
    // return true if over or using UI
    void UI();
    
    TextureProvider m_textureProvider;
    Renderer m_renderer;

    /*VelocityGen* m_velocityGenNode{};
    DensityGen* m_densityGenNode{};
    Solver* m_solverNode{};
    Display* m_displayNode{};
    Graph m_graph;
    */

    TGPU mGPU;
protected:
    //FlooidUI m_ui;
};
