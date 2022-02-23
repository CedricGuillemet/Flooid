class Flooid
{
public:
    Flooid();

    struct Parameters
    {
        int m_iterationCount;
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
	bgfx::ProgramHandle m_drawRTProgram;
    bgfx::FrameBufferHandle m_RT1;
    bgfx::FrameBufferHandle m_RT2;
    
    
    bgfx::UniformHandle m_brushUniform;
    bgfx::UniformHandle m_brushDirectionUniform;
    bgfx::UniformHandle m_brushColorUniform;
    bgfx::UniformHandle m_alphaUniform;
    bgfx::UniformHandle m_betaUniform;
    
    bgfx::UniformHandle m_texVelocityUniform;
    bgfx::UniformHandle m_texAdvectUniform;
    bgfx::UniformHandle m_texColorUniform;
    bgfx::UniformHandle m_texPressureUniform;

    bgfx::ProgramHandle m_renderRTProgram;
    bgfx::ProgramHandle m_advectProgram;
    bgfx::ProgramHandle m_divergenceProgram;
    bgfx::ProgramHandle m_gradientProgram;
    bgfx::ProgramHandle m_jacobiProgram;
    bgfx::ProgramHandle m_paintDensityProgram;
    bgfx::ProgramHandle m_paintVelocityProgram;
};
