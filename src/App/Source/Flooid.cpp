#include "common.h"
#include "bgfx/bgfx.h"
#include "Flooid.h"
#include <stdint.h>
#include "Shaders.h"

/*
 - node based solving
 - vorticity node
 - generator node
 - speed node
 - paint density node
 - paint speed node
 - solver nodes
 - display node
 - gizmo
 - rendering
 - 3D / camera
 - node based graphics editing
 - save/load json
 */

bgfx::VertexLayout Flooid::QuadVertex::ms_layout;
Flooid::Flooid()
{
}

const int TEX_SIZE = 256;
void Flooid::Init()
{
    QuadVertex::Init();

    static QuadVertex quadVertices[] =
    {
        {-1.0f,  1.0f },
        { 1.0f,  1.0f },
        {-1.0f, -1.0f },
        { 1.0f, -1.0f },
    };
    static const uint16_t quadIndices[] =
    {
        0,  2,  1,
        1,  2,  3,
    };

    m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(quadVertices, sizeof(quadVertices)), QuadVertex::ms_layout);
    m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(quadIndices, sizeof(quadIndices) ) );

    m_densityTexture = m_textureProvider.Acquire();
    m_velocityTexture = m_textureProvider.Acquire();
    
    m_brushUniform = bgfx::createUniform("brush", bgfx::UniformType::Vec4);
    m_brushDirectionUniform = bgfx::createUniform("brushDirection", bgfx::UniformType::Vec4);
    m_brushColorUniform = bgfx::createUniform("brushColor", bgfx::UniformType::Vec4);
    m_jacobiParametersUniform = bgfx::createUniform("jacobiParameters", bgfx::UniformType::Vec4);
    m_advectionUniform = bgfx::createUniform("advection", bgfx::UniformType::Vec4);
    m_curlUniform = bgfx::createUniform("curl", bgfx::UniformType::Vec4);
    m_epsilonUniform = bgfx::createUniform("epsilon", bgfx::UniformType::Vec4);
    m_positionUniform = bgfx::createUniform("position", bgfx::UniformType::Vec4);

    m_texVelocityUniform = bgfx::createUniform("s_texVelocity", bgfx::UniformType::Sampler);
    m_texAdvectUniform = bgfx::createUniform("s_texAdvect", bgfx::UniformType::Sampler);
    m_texColorUniform = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    m_texPressureUniform = bgfx::createUniform("s_texPressure", bgfx::UniformType::Sampler);
    m_texDensityUniform = bgfx::createUniform("s_texDensity", bgfx::UniformType::Sampler);
    m_texJacoviUniform = bgfx::createUniform("s_texJacobi", bgfx::UniformType::Sampler);
    m_texDivergenceUniform = bgfx::createUniform("s_texDivergence", bgfx::UniformType::Sampler);
    m_texVorticityUniform = bgfx::createUniform("s_texVorticity", bgfx::UniformType::Sampler);
    

    m_renderRTProgram = App::LoadProgram("Quad_vs", "RenderRT_fs");
    m_paintDensityProgram = App::LoadProgram("Quad_vs", "PaintDensity_fs");
    m_paintVelocityProgram = App::LoadProgram("Quad_vs", "PaintVelocity_fs");

    m_jacobiCSProgram = App::LoadProgram("Jacobi_cs", nullptr);
    m_divergenceCSProgram = App::LoadProgram("Divergence_cs", nullptr);
    m_gradientCSProgram = App::LoadProgram("Gradient_cs", nullptr);
    m_advectCSProgram = App::LoadProgram("Advect_cs", nullptr);
    m_vorticityCSProgram = App::LoadProgram("Vorticity_cs", nullptr);
    m_vorticityForceCSProgram = App::LoadProgram("VorticityForce_cs", nullptr);
    m_densityGenCSProgram = App::LoadProgram("DensityGen_cs", nullptr);
}

void Flooid::Tick(const Parameters& parameters)
{
    const uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;

    // uniforms
    float brushColor[4] = { 1.f, 1.f, 1.f, 1.f };
    bgfx::setUniform(m_brushColorUniform, brushColor);
    float brushDirection[4] = {parameters.dx, parameters.dy, 0.f, 0.f};
    bgfx::setUniform(m_brushDirectionUniform, brushDirection);
    float advection[4] = {1.f, 1.f, 1.f, 1.f};
    bgfx::setUniform(m_advectionUniform, advection);

    float epsilon[4] = { 0.0002f, 1.f, 1.f, 1.f };
    bgfx::setUniform(m_epsilonUniform, epsilon);
    float curl[4] = { 2.8f, 2.8f, 1.f, 1.f };
    bgfx::setUniform(m_curlUniform, curl);

    float position[4] = {0.5f, 0.9f, 0.f, 0.1f};
    bgfx::setUniform(m_positionUniform, position);
    

    // jacobi
    float jacobiParameters[4] = { -1.f, 4.f, 0.f, 0.f };
    bgfx::setUniform(m_jacobiParametersUniform, jacobiParameters);
    
    // paint density
    float brushDensity[4] = { parameters.x, parameters.y, 0.1f, parameters.lButDown ? 0.1f : 0.f };
    bgfx::setUniform(m_brushUniform, brushDensity);
    m_densityTexture->BindAsTarget(1);
    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);
    bgfx::setState(state | BGFX_STATE_BLEND_ADD);
    bgfx::submit(1, m_paintDensityProgram);
    
    // paint velocity
    float brushVelocity[4] = { parameters.x, parameters.y, 0.1f, parameters.rButDown ? 0.5f : 0.f };
    bgfx::setUniform(m_brushUniform, brushVelocity);
    m_velocityTexture->BindAsTarget(2);
    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);
    bgfx::setState(state | BGFX_STATE_BLEND_ADD);
    bgfx::submit(2, m_paintVelocityProgram);
    
    // bunch of CS
    bgfx::setViewFrameBuffer(5, { bgfx::kInvalidHandle });

    // advect paint
    Texture* advectedDensity = m_textureProvider.Acquire();
    bgfx::setTexture(0, m_texVelocityUniform, m_velocityTexture->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setTexture(1, m_texAdvectUniform, m_densityTexture->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setImage(2, advectedDensity->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(5, m_advectCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    // advect velocity
    Texture* advectedVelocity = m_textureProvider.Acquire();
    bgfx::setTexture(0, m_texVelocityUniform, m_velocityTexture->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setTexture(1, m_texAdvectUniform, m_velocityTexture->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setImage(2, advectedVelocity->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(5, m_advectCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    // density gen
    bgfx::setImage(0, advectedDensity->GetTexture(), 0, bgfx::Access::ReadWrite);
    bgfx::dispatch(5, m_densityGenCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    // vorticity
    if (1)
    {
        Texture* vorticity = m_textureProvider.Acquire();
        bgfx::setTexture(0, m_texVelocityUniform, advectedVelocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setImage(1, vorticity->GetTexture(), 0, bgfx::Access::Write);
        bgfx::dispatch(5, m_vorticityCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

        Texture* vorticityForce = m_textureProvider.Acquire();
        bgfx::setTexture(0, m_texVelocityUniform, advectedVelocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setTexture(1, m_texVorticityUniform, vorticity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setImage(2, vorticityForce->GetTexture(), 0, bgfx::Access::Write);
        bgfx::dispatch(5, m_vorticityForceCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
        m_textureProvider.Release(vorticity);
        m_textureProvider.Release(advectedVelocity);
        advectedVelocity = vorticityForce;
    }

    // divergence
    Texture* divergence = m_textureProvider.Acquire();
    bgfx::setTexture(0, m_texVelocityUniform, advectedVelocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(1, divergence->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(5, m_divergenceCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    // clear density
    Texture* jacobi[2] = {m_textureProvider.Acquire(), m_textureProvider.Acquire()};
    jacobi[0]->BindAsTarget(6);
    bgfx::setViewClear(6, BGFX_CLEAR_COLOR, 0x00000000);
    bgfx::touch(6);

    bgfx::setViewFrameBuffer(7, {bgfx::kInvalidHandle});
    bgfx::setViewRect(7, 0, 0, uint16_t(TEX_SIZE), uint16_t(TEX_SIZE));
    bgfx::setViewClear(7, BGFX_CLEAR_COLOR, 0x00000000);
    bgfx::touch(7);
    
    // jacobi
    for(int i = 0; i < parameters.m_iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;
        
        bgfx::setTexture(0, m_texJacoviUniform, jacobi[indexSource]->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setTexture(1, m_texDivergenceUniform, divergence->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setImage(2, jacobi[indexDestination]->GetTexture(), 0, bgfx::Access::Write);
        bgfx::dispatch(7, m_jacobiCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    }
    const int lastJacobiIndex = parameters.m_iterationCount & 1;
    m_textureProvider.Release(divergence);
    
    // gradient
    bgfx::setViewFrameBuffer(8, { bgfx::kInvalidHandle });
    bgfx::setTexture(0, m_texPressureUniform, jacobi[lastJacobiIndex]->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setTexture(1, m_texVelocityUniform, advectedVelocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(2, m_velocityTexture->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(8, m_gradientCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    
    m_textureProvider.Release(jacobi[0]);
    m_textureProvider.Release(jacobi[1]);
    m_textureProvider.Release(advectedVelocity);
    
    // draw RT
    //bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));
    bgfx::setViewFrameBuffer(0, {bgfx::kInvalidHandle});
    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);
    bgfx::setState(state);
    bgfx::setTexture(0, m_texColorUniform, m_densityTexture->GetTexture());
    bgfx::submit(0, m_renderRTProgram);
    
    // swap advect/vel
    m_textureProvider.Release(m_densityTexture);
    m_densityTexture = advectedDensity;
}
