#include "tgpu.h"

static const int TEX_SIZE = 256;
TGPU::TGPU()
{

}

void TGPU::DensityGen(TextureProvider& textureProvider)
{
    Imm::matrix m_matrix;
    m_matrix.translation(0.5f, 0.1f, 0.f);
    
    Imm::matrix invWorld;
    invWorld.inverse(m_matrix);
    bgfx::setUniform(m_invWorldMatrixUniform, invWorld.m16);
    
    bgfx::setImage(0, m_densityTexture->GetTexture(), 0, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), m_densityGenCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
}

void TGPU::VelocityGen(TextureProvider& textureProvider)
{
    Imm::vec3 m_position{0.5f, 0.1f, 0.f};
    float m_radius = 0.05;
    float position[4] = { m_position.x, m_position.y, m_position.z, m_radius };
    bgfx::setUniform(m_positionUniform, position);

    Imm::matrix matrix;
    Imm::vec3 scale{m_radius, m_radius, m_radius};
    //ImGuizmo::RecomposeMatrixFromComponents(&m_position.x, &m_orientation.x, &scale.x, matrix.m16);
    float direction[4] = { 0.f, 0.1f, 0.f, 0.f};//matrix.dir.x, matrix.dir.y, matrix.dir.z, 0.f };
    bgfx::setUniform(m_directionUniform, direction);

    bgfx::setImage(0, m_velocityTexture->GetTexture(), 0, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), m_velocityGenCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
}

void TGPU::Advect(TextureProvider& textureProvider, Texture* source, Texture* velocity, Texture* output)
{
    float advection[4] = { 1.f/*m_timeScale*/, 0.998f/*m_dissipation*/, 0.f, 0.f };
    bgfx::setUniform(m_advectionUniform, advection);

    bgfx::setTexture(0, m_texVelocityUniform, velocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setTexture(1, m_texAdvectUniform, source->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setImage(2, output->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_advectCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
}

void TGPU::Gradient(TextureProvider& textureProvider, Texture* u, Texture* velocity, Texture* destination)
{
    bgfx::setTexture(0, m_texPressureUniform, u->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setTexture(1, m_texVelocityUniform, velocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(2, destination->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_gradientCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
}

void TGPU::Divergence(TextureProvider& textureProvider, Texture* velocity, Texture* destination)
{
    bgfx::setTexture(0, m_texVelocityUniform, velocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(1, destination->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_divergenceCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
}

void TGPU::Jacobi(TextureProvider& textureProvider, Texture* jacobi[2], Texture* rhs, int iterationCount)
{
    float hsq = 1.f;
    float jacobiParameters[4] = { hsq, 0.f, 0.f, 0.f };
    bgfx::setUniform(m_jacobiParametersUniform, jacobiParameters);

    for (int i = 0; i < iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;

        bgfx::setTexture(0, m_texUUniform, jacobi[indexSource]->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setTexture(1, m_texRHSUniform, rhs->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setImage(2, jacobi[indexDestination]->GetTexture(), 0, bgfx::Access::Write);
        bgfx::dispatch(textureProvider.GetViewId(), m_jacobiCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    }
}


void TGPU::Init(TextureProvider& textureProvider)
{
    m_densityTexture = textureProvider.Acquire(PlugType::Any, 256);
    m_velocityTexture = textureProvider.Acquire(PlugType::Any, 256);
    
    m_densityGenCSProgram = App::LoadProgram("DensityGen_cs", nullptr);
    m_invWorldMatrixUniform = bgfx::createUniform("invWorldMatrix", bgfx::UniformType::Mat4);


    m_velocityGenCSProgram = App::LoadProgram("VelocityGen_cs", nullptr);
    m_positionUniform = bgfx::createUniform("position", bgfx::UniformType::Vec4);
    m_directionUniform = bgfx::createUniform("direction", bgfx::UniformType::Vec4);


    m_jacobiCSProgram = App::LoadProgram("Jacobi_cs", nullptr);
    m_divergenceCSProgram = App::LoadProgram("Divergence_cs", nullptr);
    m_gradientCSProgram = App::LoadProgram("Gradient_cs", nullptr);
    m_clearCSProgram = App::LoadProgram("Clear_cs", nullptr);
    m_jacobiParametersUniform = bgfx::createUniform("jacobiParameters", bgfx::UniformType::Vec4);
    m_texVelocityUniform = bgfx::createUniform("s_texVelocity", bgfx::UniformType::Sampler);
    m_texJacoviUniform = bgfx::createUniform("s_texJacobi", bgfx::UniformType::Sampler);
    m_texDivergenceUniform = bgfx::createUniform("s_texDivergence", bgfx::UniformType::Sampler);
    m_texPressureUniform = bgfx::createUniform("s_texPressure", bgfx::UniformType::Sampler);

    m_advectCSProgram = App::LoadProgram("Advect_cs", nullptr);
    m_advectionUniform = bgfx::createUniform("advection", bgfx::UniformType::Vec4);
    m_texAdvectUniform = bgfx::createUniform("s_texAdvect", bgfx::UniformType::Sampler);


    m_downscaleCSProgram = App::LoadProgram("Downscale_cs", nullptr);
    m_upscaleCSProgram = App::LoadProgram("Upscale_cs", nullptr);
    m_residualCSProgram = App::LoadProgram("Residual_cs", nullptr);

    m_texUUniform = bgfx::createUniform("s_texU", bgfx::UniformType::Sampler);
    m_texRHSUniform = bgfx::createUniform("s_texRHS", bgfx::UniformType::Sampler);

    m_invhsqUniform = bgfx::createUniform("invhsq", bgfx::UniformType::Vec4);
    m_fineTexSizeUniform = bgfx::createUniform("fineTexSize", bgfx::UniformType::Vec4);
}

void TGPU::Tick(TextureProvider& textureProvider)
{
    VelocityGen(textureProvider);
    DensityGen(textureProvider);

    // advection

    Texture* advectedVelocity = textureProvider.Acquire(PlugType::Any, 256);
    Texture* newDensity = textureProvider.Acquire(PlugType::Any, TEX_SIZE);
    Advect(textureProvider, m_densityTexture, m_velocityTexture, newDensity);
    textureProvider.Release(m_densityTexture);
    
    Advect(textureProvider, m_velocityTexture, m_velocityTexture, advectedVelocity);
    
    
    Texture* tempRHS = textureProvider.Acquire(PlugType::Any, TEX_SIZE);
    Divergence(textureProvider, advectedVelocity, tempRHS);
    
    
    Texture* jacobi[2] = {textureProvider.Acquire(PlugType::Any, TEX_SIZE), textureProvider.Acquire(PlugType::Any, TEX_SIZE)};
    bgfx::setImage(0, jacobi[0]->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_clearCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    Jacobi(textureProvider, jacobi, tempRHS, 1000);
    
    textureProvider.Release(tempRHS);
    textureProvider.Release(jacobi[1]);
    
    Texture* newVelocity = textureProvider.Acquire(PlugType::Any, TEX_SIZE);
    Gradient(textureProvider, jacobi[0], advectedVelocity, newVelocity);
    
    textureProvider.Release(jacobi[0]);
    
    textureProvider.Release(advectedVelocity);
    
    textureProvider.Release(m_velocityTexture);
    
    m_densityTexture = newDensity;
    m_velocityTexture = newVelocity;
}
