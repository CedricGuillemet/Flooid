#include "TextureProvider.h"
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
    void Tick(const Parameters& parameters);
    

    struct QuadVertex
    {
        float u, v;
        static void Init()
        {
            ms_layout
                .begin()
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .end();
        }

        static bgfx::VertexLayout ms_layout;
    };
	bgfx::VertexBufferHandle m_vbh;
	bgfx::IndexBufferHandle m_ibh;
/*
    bgfx::FrameBufferHandle m_RT1;
    bgfx::FrameBufferHandle m_RT2;
    bgfx::FrameBufferHandle m_RT1adv;
    bgfx::FrameBufferHandle m_RT2adv;
    bgfx::FrameBufferHandle m_RTdivergence;
    bgfx::FrameBufferHandle m_RTjacobi[2];
*/
    Texture* m_densityTexture;
    Texture* m_velocityTexture;
    
    bgfx::UniformHandle m_brushUniform;
    bgfx::UniformHandle m_brushDirectionUniform;
    bgfx::UniformHandle m_brushColorUniform;
    bgfx::UniformHandle m_jacobiParametersUniform;
    bgfx::UniformHandle m_advectionUniform;
    bgfx::UniformHandle m_curlUniform;
    bgfx::UniformHandle m_epsilonUniform;
    bgfx::UniformHandle m_positionUniform;

    bgfx::UniformHandle m_texVelocityUniform;
    bgfx::UniformHandle m_texAdvectUniform;
    bgfx::UniformHandle m_texColorUniform;
    bgfx::UniformHandle m_texPressureUniform;
    bgfx::UniformHandle m_texDensityUniform;
    bgfx::UniformHandle m_texVorticityUniform;
    //bgfx::UniformHandle m_texDensityUniform;

    bgfx::UniformHandle m_texJacoviUniform;
    bgfx::UniformHandle m_texDivergenceUniform;

    bgfx::ProgramHandle m_renderRTProgram;
    bgfx::ProgramHandle m_paintDensityProgram;
    bgfx::ProgramHandle m_paintVelocityProgram;

    bgfx::ProgramHandle m_divergenceCSProgram;
    bgfx::ProgramHandle m_gradientCSProgram;
    bgfx::ProgramHandle m_jacobiCSProgram;
    bgfx::ProgramHandle m_advectCSProgram;
    bgfx::ProgramHandle m_vorticityCSProgram;
    bgfx::ProgramHandle m_vorticityForceCSProgram;
    bgfx::ProgramHandle m_densityGenCSProgram;
    
    TextureProvider m_textureProvider;
};
