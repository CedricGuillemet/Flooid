#include "GraphNode.h"
#include "TextureProvider.h"
#include "Shaders.h"
#include "imgui.h"
#include "GraphEditorDelegate.h"
#include "UIGizmos.h"
#include <algorithm>

const int TEX_SIZE = 256;

void GraphNode::SetOutput(unsigned int outputIndex, Texture* texture)
{
    texture->m_type = GetOutputTypes()[outputIndex];
    m_outputs[outputIndex] = texture;
}

// -----------------------------------------------------------------------------------------------------

void DensityGen::Init()
{
    m_inputTypes = {PlugType::Particles};
    m_outputTypes = {PlugType::Particles };

    m_densityGenCSProgram = App::LoadProgram("DensityGen_cs", nullptr);
    m_invWorldMatrixUniform = bgfx::createUniform("invWorldMatrix", bgfx::UniformType::Mat4);

    GraphEditorDelegate::mTemplateFunctions.push_back(GetTemplate);
    _nodeType = GraphNode::_runtimeType++;
}

void DensityGen::Tick(TextureProvider& textureProvider)
{
    //float position[4] = { m_position.x, m_position.y, m_position.z, m_radius };
    //bgfx::setUniform(m_positionUniform, position);

    Imm::matrix invWorld;
    invWorld.inverse(m_matrix);
    bgfx::setUniform(m_invWorldMatrixUniform, invWorld.m16);
    
    auto density = GetInput(0);
    
    // clear
    //bgfx::setImage(0, density->GetTexture(), 0, bgfx::Access::Write);
    //bgfx::dispatch(textureProvider.GetViewId(), Solver::m_clearCSProgram, TEX_SIZE / 16, TEX_SIZE / 16, TEX_SIZE / 4);

    bgfx::setImage(0, density->GetTexture(), 0, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), m_densityGenCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    SetOutput(0, density);
}

bool DensityGen::UI(UIGizmos& uiGizmos)
{
    //bool changed = ImGui::InputFloat("Radius", &m_radius);
    uiGizmos.Edit(&m_matrix);
    uiGizmos.AddSphere(m_matrix);
    return false;
}

// -----------------------------------------------------------------------------------------------------

void VelocityGen::Init()
{
    m_inputTypes = {PlugType::Velocity};
    m_outputTypes = {PlugType::Velocity};

    m_velocityGenCSProgram = App::LoadProgram("VelocityGen_cs", nullptr);
    m_positionUniform = bgfx::createUniform("position", bgfx::UniformType::Vec4);
    m_directionUniform = bgfx::createUniform("direction", bgfx::UniformType::Vec4);

    GraphEditorDelegate::mTemplateFunctions.push_back(GetTemplate);
    _nodeType = GraphNode::_runtimeType++;
}

void VelocityGen::Tick(TextureProvider& textureProvider)
{
    float position[4] = { m_position.x, m_position.y, m_position.z, m_radius };
    bgfx::setUniform(m_positionUniform, position);
    
    Imm::matrix matrix;
    Imm::vec3 scale{m_radius, m_radius, m_radius};
    ImGuizmo::RecomposeMatrixFromComponents(&m_position.x, &m_orientation.x, &scale.x, matrix.m16);
    float direction[4] = { matrix.dir.x, matrix.dir.y, matrix.dir.z, 0.f };
    bgfx::setUniform(m_directionUniform, direction);

    auto velocity = GetInput(0);
    bgfx::setImage(0, velocity->GetTexture(), 0, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), m_velocityGenCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    SetOutput(0, velocity);
}

bool VelocityGen::UI(UIGizmos& uiGizmos)
{
    bool changed = ImGui::InputFloat("Radius", &m_radius);
    uiGizmos.Edit(&m_position, &m_orientation, &m_radius);
    Imm::matrix sphereMatrix;
    sphereMatrix.translationScale({ m_position.x, m_position.y, m_position.z }, { m_radius, m_radius, m_radius });
    uiGizmos.AddSphere(sphereMatrix);
    return changed;
}

// -----------------------------------------------------------------------------------------------------

void Solver::Init()
{
    m_inputTypes = { PlugType::Particles, PlugType::Velocity };
    m_outputTypes = { PlugType::Particles, PlugType::Velocity };

    m_jacobiCSProgram = App::LoadProgram("Jacobi_cs", nullptr);
    m_divergenceCSProgram = App::LoadProgram("Divergence_cs", nullptr);
    m_gradientCSProgram = App::LoadProgram("Gradient_cs", nullptr);
    m_clearCSProgram = App::LoadProgram("Clear_cs", nullptr);
    m_jacobiParametersUniform = bgfx::createUniform("jacobiParameters", bgfx::UniformType::Vec4);
    m_texVelocityUniform = bgfx::createUniform("s_texVelocity", bgfx::UniformType::Sampler);
    m_texJacoviUniform = bgfx::createUniform("s_texJacobi", bgfx::UniformType::Sampler);
    m_texDivergenceUniform = bgfx::createUniform("s_texDivergence", bgfx::UniformType::Sampler);

    // vorticity
    m_vorticityCSProgram = App::LoadProgram("Vorticity_cs", nullptr);
    m_vorticityForceCSProgram = App::LoadProgram("VorticityForce_cs", nullptr);
    m_texVorticityUniform = bgfx::createUniform("s_texVorticity", bgfx::UniformType::Sampler);
    m_curlUniform = bgfx::createUniform("curl", bgfx::UniformType::Vec4);
    m_epsilonUniform = bgfx::createUniform("epsilon", bgfx::UniformType::Vec4);

    // advection
    m_advectCSProgram = App::LoadProgram("Advect_cs", nullptr);
    m_advectionUniform = bgfx::createUniform("advection", bgfx::UniformType::Vec4);
    m_texAdvectUniform = bgfx::createUniform("s_texAdvect", bgfx::UniformType::Sampler);

    
    // V cycle
    const auto texFormat = bgfx::TextureFormat::RGBA16F;
    
    m_downscaleCSProgram = App::LoadProgram("Downscale_cs", nullptr);
    m_upscaleCSProgram = App::LoadProgram("Upscale_cs", nullptr);
    m_residualCSProgram = App::LoadProgram("Residual_cs", nullptr);
    
    m_texUUniform = bgfx::createUniform("s_texU", bgfx::UniformType::Sampler);
    m_texRHSUniform = bgfx::createUniform("s_texRHS", bgfx::UniformType::Sampler);

    m_invhsqUniform = bgfx::createUniform("invhsq", bgfx::UniformType::Vec4);
    m_fineTexSizeUniform = bgfx::createUniform("fineTexSize", bgfx::UniformType::Vec4);
    
    GraphEditorDelegate::mTemplateFunctions.push_back(GetTemplate);
    _nodeType = GraphNode::_runtimeType++;
}

void Solver::Advect(TextureProvider& textureProvider, Texture* source, Texture* velocity, Texture* output)
{
    float advection[4] = { m_timeScale, m_dissipation, 0.f, 0.f };
    bgfx::setUniform(m_advectionUniform, advection);

    bgfx::setTexture(0, m_texVelocityUniform, velocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setTexture(1, m_texAdvectUniform, source->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setImage(2, output->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_advectCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
}

void Solver::IterateJacobi(int iterationCount, TextureProvider& textureProvider, Texture* jacobi[2], Texture* divergence)
{
    for (int i = 0; i < iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;

        bgfx::setTexture(0, m_texJacoviUniform, jacobi[indexSource]->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setTexture(1, m_texDivergenceUniform, divergence->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setImage(2, jacobi[indexDestination]->GetTexture(), 0, bgfx::Access::Write);
        bgfx::dispatch(textureProvider.GetViewId(), m_jacobiCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    }
}

void Solver::IterateJacobi(int iterationCount, TextureProvider& textureProvider, bgfx::FrameBufferHandle jacobi[2], bgfx::FrameBufferHandle divergence)
{
    for (int i = 0; i < iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;

        bgfx::setTexture(0, m_texJacoviUniform, bgfx::getTexture(jacobi[indexSource]), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setTexture(1, m_texDivergenceUniform, bgfx::getTexture(divergence), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setImage(2, bgfx::getTexture(jacobi[indexDestination]), 0, bgfx::Access::Write);
        bgfx::dispatch(textureProvider.GetViewId(), m_jacobiCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
    }
}


void Solver::JacobiStep(TextureProvider& textureProvider, const Texture* u, const Texture* rhs, Texture* destination, float hsq)
{
/*    assert(source.mComponentCount == 1);
    //assert(divergence.mComponentCount == 1);
    assert(destination.mComponentCount == 1);
    //assert(source.mSize == divergence.mSize);
    assert(source.mSize == destination.mSize);
    
    for (int y = 1; y < source.mSize - 1; y++)
    {
        for (int x = 1; x < source.mSize - 1; x++)
        {
            int index = (y * source.mSize + x);

            const float omega = 4.f / 5.f;
            //float hsq = 1.f / 128.f;
            //hsq *= hsq;
            float value = source.mBuffer[index] + omega * 0.25f * (-hsq * rhs.mBuffer[index] +
                source.mBuffer[index - 1] +
                source.mBuffer[index + 1] +
                source.mBuffer[index - source.mSize] +
                source.mBuffer[index + source.mSize] -
                4.f * source.mBuffer[index]
            );
            float* pd = &destination.mBuffer[index];
            pd[0] = value;
        }
    }*/
    
    bgfx::setTexture(0, m_texUUniform, u->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setTexture(1, m_texRHSUniform, rhs->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(2, destination->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_jacobiCSProgram, destination->m_size / 16, destination->m_size / 16);

}

void Solver::Jacobi(TextureProvider& textureProvider, Texture* u, const Texture* rhs, int iterationCount, float hsq)
{
    /*assert(u.mComponentCount == 1);
    assert(rhs.mComponentCount == 1);

    Buf jacobiBuf(u.mSize, 1);
    
    Buf* jacobis[2] = {&u, &jacobiBuf};
    iterationCount &= ~1;
    for (int i = 0; i < iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;

        JacobiStep(*jacobis[indexSource], rhs, *jacobis[indexDestination], hsq);
    }*/
    
    float jacobiParameters[4] = { hsq, 0.f, 0.f, 0.f };
    bgfx::setUniform(m_jacobiParametersUniform, jacobiParameters);

    
    Texture* jacobiBuf = textureProvider.Acquire(PlugType::Any, u->m_size);
    
    Texture* jacobis[2] = {u, jacobiBuf};
    iterationCount &= ~1;
    for (int i = 0; i < iterationCount; i++)
    {
        const int indexSource = i & 1;
        const int indexDestination = (i + 1) & 1;

        JacobiStep(textureProvider, jacobis[indexSource], rhs, jacobis[indexDestination], hsq);
    }
    textureProvider.Release(jacobiBuf);
}

void Solver::coarsen(TextureProvider& textureProvider, const Texture* uf, Texture* uc)
{
    /*assert(uf.mSize == uc.mSize * 2);
    for (int jc = 1; jc < uc.mSize; jc++)
    {
      for (int ic = 1; ic < uc.mSize; ic++)
      {
          int indexDst = jc * uc.mSize + ic;
          int indexSrc = jc * 2 * uf.mSize + ic * 2;
          uc.mBuffer[indexDst] = 0.5 * uf.mBuffer[indexSrc] + 0.25 * (uf.mBuffer[indexSrc - 1] +
                                                                       uf.mBuffer[indexSrc + 1] +
                                                                       uf.mBuffer[indexSrc - uf.mSize] +
                                                                       uf.mBuffer[indexSrc + uf.mSize])
          
          + 0.125 * (uf.mBuffer[indexSrc - 1 - uf.mSize] +
                                                                      uf.mBuffer[indexSrc + 1 - uf.mSize] +
                                                                      uf.mBuffer[indexSrc - 1 + uf.mSize] +
                                                                      uf.mBuffer[indexSrc + 1 + uf.mSize])
          ;
      }
    }*/
    bgfx::setTexture(0, m_texUUniform, uf->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP| BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(1, uc->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_downscaleCSProgram, uf->m_size / 16, uf->m_size / 16);

}

void Solver::refine_and_add(TextureProvider& textureProvider, const Texture* u, Texture* uf)
{
    /*for (int jc = 1; jc < u.mSize; jc++)
    {
      for (int ic = 1; ic < u.mSize; ic++)
      {
          int indexSrc = jc * u.mSize + ic;
          int indexDst = jc * 2 * uf.mSize + ic * 2;

          int dx =(ic >= u.mSize-1) ? 0 : 1;
          int dy =(jc >= u.mSize-1) ? 0 : 1;
          float v00 = u.mBuffer[indexSrc];
          float v01 = u.mBuffer[indexSrc + dx];
          float v10 = u.mBuffer[indexSrc + dy * u.mSize];
          float v11 = u.mBuffer[indexSrc + dy * u.mSize + dx];
          
          uf.mBuffer[indexDst] += v00;
          uf.mBuffer[indexDst+1] += (v00 + v01) * 0.5;
          uf.mBuffer[indexDst+uf.mSize] += (v00 + v10) * 0.5;
          uf.mBuffer[indexDst+uf.mSize+1] += (v00 + v01 + v10 + v11) * 0.25;
      }
    }*/
    
    float refineParameters[4] = { static_cast<float>(uf->m_size), 0.f, 0.f, 0.f };
    bgfx::setUniform(m_fineTexSizeUniform, refineParameters);

    
    bgfx::setTexture(0, m_texUUniform, u->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    bgfx::setImage(1, uf->GetTexture(), 0, bgfx::Access::ReadWrite);
    bgfx::dispatch(textureProvider.GetViewId(), m_upscaleCSProgram, uf->m_size / 16, uf->m_size / 16);
}

void Solver::compute_residual(TextureProvider& textureProvider, const Texture* u, const Texture* rhs, Texture* res, float hsq)
{
    /*const float invhsq = 1.f / hsq;
    for (int jc = 1; jc < u.mSize; jc++)
    {
      for (int ic = 1; ic < u.mSize; ic++)
      {
          int index = jc * u.mSize + ic;
          int dx =(ic >= u.mSize-1) ? 0 : 1;
          int dy =(jc >= u.mSize-1) ? 0 : 1;

          res.mBuffer[index] = rhs.mBuffer[index] - (
                                                     4. * u.mBuffer[index]
                                                     - u.mBuffer[index - dx]
                                                     - u.mBuffer[index + dx]
                                                     - u.mBuffer[index - dy * u.mSize]
                                                     - u.mBuffer[index + dy * u.mSize]
                                                     ) * invhsq;
      }
    }*/
    
    float invhsq[4] = { 1.f / hsq, 0.f, 0.f, 0.f };
    bgfx::setUniform(m_invhsqUniform, invhsq);

    
    bgfx::setTexture(0, m_texUUniform, u->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setTexture(1, m_texRHSUniform, rhs->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(2, res->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_residualCSProgram, res->m_size / 16, res->m_size / 16);

}

void Solver::compute_and_coarsen_residual(TextureProvider& textureProvider, const Texture* u, const Texture* rhs, Texture* resc, float hsq)
{
    //Buf resf(u.mSize, 1);
    Texture* resf = textureProvider.Acquire(PlugType::Any, u->m_size);
    //resf.Set(0.f);
    compute_residual(textureProvider, u, rhs, resf, hsq);
    coarsen(textureProvider, resf, resc);
    textureProvider.Release(resf);
}

void Solver::vcycle(TextureProvider& textureProvider, const Texture* rhs, Texture* u, int fineSize, int level, int max)
{
    int ssteps = 4;
    float hsq = level+1;//sqrtf((level+1)*2);
    
    if (level == max)
    {
        Jacobi(textureProvider, u, rhs, 2, hsq);
        return;
    }
    
    int sizeNext = fineSize / powf(2.f, level+1);
    
    //Buf rhsNext(sizeNext, 1);
    //Buf uNext(sizeNext, 1);
    Texture* rhsNext = textureProvider.Acquire(PlugType::Any, sizeNext);
    Texture* uNext = textureProvider.Acquire(PlugType::Any, sizeNext);
    
    Jacobi(textureProvider, u, rhs, ssteps, hsq);
    compute_and_coarsen_residual(textureProvider, u, rhs, rhsNext, hsq);
    //uNext.Set(0.f);
    bgfx::setImage(0, uNext->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_clearCSProgram, sizeNext / 16, sizeNext / 16);

    
    vcycle(textureProvider, rhsNext, uNext, fineSize, level+1, max);
    textureProvider.Release(rhsNext);
    refine_and_add(textureProvider, uNext, u);
    textureProvider.Release(uNext);
    Jacobi(textureProvider, u, rhs, ssteps, hsq);
}

void Solver::Tick(TextureProvider& textureProvider)
{
    float jacobiParameters[4] = { -1.f, 4.f, 0.f, 0.f };
    bgfx::setUniform(m_jacobiParametersUniform, jacobiParameters);

    auto velocity = GetInput(1);

    // advection
    auto particles = GetInput(0);
    Texture* newParticles = textureProvider.Acquire(PlugType::Particles, TEX_SIZE);
    Texture* newVelocity = textureProvider.Acquire(PlugType::Velocity, TEX_SIZE);
    Advect(textureProvider, particles, velocity, newParticles);
    Advect(textureProvider, velocity, velocity, newVelocity);

    textureProvider.Release(textureProvider.m_densityTexture);
    textureProvider.m_densityTexture = newParticles;

    velocity = newVelocity;

    if (m_vorticityEnable)
    {
        float epsilon[4] = { m_epsilon, 0.f, 0.f, 0.f };
        bgfx::setUniform(m_epsilonUniform, epsilon);
        float curl[4] = { m_curl, m_curl, 0.f, 0.f };
        bgfx::setUniform(m_curlUniform, curl);

        Texture* vorticity = textureProvider.Acquire(PlugType::Any, TEX_SIZE);
        bgfx::setTexture(0, m_texVelocityUniform, velocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setImage(1, vorticity->GetTexture(), 0, bgfx::Access::Write);
        bgfx::dispatch(textureProvider.GetViewId(), m_vorticityCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

        Texture* vorticityForce = textureProvider.Acquire(PlugType::Velocity, TEX_SIZE);
        bgfx::setTexture(0, m_texVelocityUniform, velocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setTexture(1, m_texVorticityUniform, vorticity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
        bgfx::setImage(2, vorticityForce->GetTexture(), 0, bgfx::Access::Write);
        bgfx::dispatch(textureProvider.GetViewId(), m_vorticityForceCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);
        textureProvider.Release(vorticity);
        textureProvider.Release(velocity);
        velocity = vorticityForce;
    }
    // divergence
    Texture* divergence = textureProvider.Acquire(PlugType::Any, TEX_SIZE);
    bgfx::setTexture(0, m_texVelocityUniform, velocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(1, divergence->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_divergenceCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    
    
    
    
    
    Texture* u = textureProvider.Acquire(PlugType::Any, 256);
    vcycle(textureProvider, divergence, u, 256, 0, 0);
    
    textureProvider.Release(divergence);
    
    
    
    
    
    // gradient
    Texture* outputVelocity = textureProvider.Acquire(PlugType::Velocity, TEX_SIZE);
    bgfx::setTexture(0, m_texPressureUniform, u->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setTexture(1, m_texVelocityUniform, velocity->GetTexture(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    bgfx::setImage(2, outputVelocity->GetTexture(), 0, bgfx::Access::Write);
    bgfx::dispatch(textureProvider.GetViewId(), m_gradientCSProgram, TEX_SIZE / 16, TEX_SIZE / 16);

    textureProvider.Release(u);
    //textureProvider.Release(jacobi[1]);
    textureProvider.Release(velocity);

    // swap
    textureProvider.Release(textureProvider.m_velocityTexture);
    textureProvider.m_velocityTexture = outputVelocity;

    // out
    SetOutput(0, textureProvider.m_densityTexture);
    SetOutput(0, textureProvider.m_velocityTexture);
}

bool Solver::UI(UIGizmos& uiGizmos)
{
    bool changed = ImGui::InputFloat("Alpha", &m_alpha);
    changed |= ImGui::InputFloat("Beta", &m_beta);

    changed |= ImGui::InputInt("Iterations", &m_iterationCount);
    m_iterationCount = std::max(m_iterationCount, 1);
    m_iterationCount = std::min(m_iterationCount, 10000);

    ImGui::NewLine(); // advection
    changed |= ImGui::InputFloat("Time scale", &m_timeScale);
    changed |= ImGui::InputFloat("Dissipation", &m_dissipation);

    ImGui::NewLine();
    changed |= ImGui::Checkbox("Vorticity", &m_vorticityEnable);
    if (m_vorticityEnable)
    {
        changed |= ImGui::InputFloat("Epsilon", &m_epsilon);
        changed |= ImGui::InputFloat("Curl", &m_curl);
    }
    return changed;
}

// -----------------------------------------------------------------------------------------------------

void Display::Init()
{
    m_inputTypes = {PlugType::Particles };
    m_outputTypes = {PlugType::Image};

    GraphEditorDelegate::mTemplateFunctions.push_back(GetTemplate);
    _nodeType = GraphNode::_runtimeType++;
}

void Display::Tick(TextureProvider& textureProvider)
{
    //SetOutput(0, GetInput(0));
}

bool Display::UI(UIGizmos& uiGizmos)
{
    uiGizmos.Edit(&m_lightPosition);
    return false;
}

// -----------------------------------------------------------------------------------------------------

GraphNode* Graph::GetSelectedNode() const
{
    GraphNode* res{nullptr};
    for(auto node : m_nodes)
    {
        if(!node->m_selected)
        {
            continue;
        }
        if(!res)
        {
            res = node;
        }
        else
        {
            return nullptr;
        }
    }
    return res;
}

void Graph::RecurseLayout(std::vector<NodePosition>& positions,
    std::map<int, int>& stacks,
    size_t currentIndex,
    int currentLayer)
{
    const auto& nodes = m_nodes;
    const auto& links = m_links;

    if (positions[currentIndex].mLayer == -1)
    {
        positions[currentIndex].mLayer = currentLayer;
        int layer = positions[currentIndex].mLayer = currentLayer;
        if (stacks.find(layer) != stacks.end())
        {
            stacks[layer]++;
        }
        else
        {
            stacks[layer] = 0;
        }
        positions[currentIndex].mStackIndex = stacks[currentLayer];
    }
    else
    {
        // already hooked node
        if (currentLayer > positions[currentIndex].mLayer)
        {
            // remove stack at current pos
            int currentStack = positions[currentIndex].mStackIndex;
            for (auto& pos : positions)
            {
                if (pos.mLayer == positions[currentIndex].mLayer && pos.mStackIndex > currentStack)
                {
                    pos.mStackIndex--;
                    stacks[pos.mLayer]--;
                }
            }
            // apply new one
            int layer = positions[currentIndex].mLayer = currentLayer;
            if (stacks.find(layer) != stacks.end())
                stacks[layer]++;
            else
                stacks[layer] = 0;
            positions[currentIndex].mStackIndex = stacks[currentLayer];
        }
    }

    size_t InputsCount = nodes[currentIndex]->GetInputCount();
    //gMetaNodes[nodes[currentIndex].mNodeType].mInputs.size();
    std::vector<int> inputNodes(InputsCount, -1);
    for (auto& link : links)
    {
        if (link.m_outputNodeIndex != currentIndex)
        {
            continue;
        }
        inputNodes[link.m_outputSlotIndex] = link.m_inputNodeIndex;
    }
    for (auto inputNode : inputNodes)
    {
        if (inputNode == -1)
        {
            continue;
        }
        RecurseLayout(positions, stacks, inputNode, currentLayer + 1);
    }
}

void Graph::Layout(const std::vector<size_t>& orderList)
{
    if (m_nodes.empty())
    {
        return;
    }
    // get stack/layer pos
    std::vector<NodePosition> nodePositions(m_nodes.size(), { -1, -1, -1 });
    std::map<int, int> stacks;
    ImRect sourceRect, destRect;

    std::vector<ImVec2> nodePos(m_nodes.size());

    // compute source bounds
    for (unsigned int i = 0; i < m_nodes.size(); i++)
    {
        const auto& node = *m_nodes[i];
        sourceRect.Add(ImRect({node.m_x, node.m_y}, {node.m_x + 100, node.m_y + 100}));
    }

    for (unsigned int i = 0; i < m_nodes.size(); i++)
    {
        size_t nodeIndex = orderList[m_nodes.size() - i - 1];
        RecurseLayout(nodePositions, stacks, nodeIndex, 0);
    }

    // set corresponding node index in nodePosition
    for (unsigned int i = 0; i < m_nodes.size(); i++)
    {
        int nodeIndex = int(orderList[i]);
        auto& layout = nodePositions[nodeIndex];
        layout.mNodeIndex = nodeIndex;
    }

    // sort nodePositions
    std::sort(nodePositions.begin(), nodePositions.end());

    // set x,y position from layer/stack
    float currentStackHeight = 0.f;
    int currentLayerIndex = -1;
    for (unsigned int i = 0; i < nodePositions.size(); i++)
    {
        auto& layout = nodePositions[i];
        if (currentLayerIndex != layout.mLayer)
        {
            currentLayerIndex = layout.mLayer;
            currentStackHeight = 0.f;
        }
        size_t nodeIndex = layout.mNodeIndex;
        const auto& node = m_nodes[nodeIndex];
        float height = 120;//float(gMetaNodes[node.mNodeType].mHeight);
        nodePos[nodeIndex] = ImVec2(-layout.mLayer * 280.f, currentStackHeight);
        currentStackHeight += height + 40.f;
    }

    // new bounds
    for (unsigned int i = 0; i < m_nodes.size(); i++)
    {
        ImVec2 newPos = nodePos[i];
        // todo: support height more closely with metanodes
        destRect.Add(ImRect(newPos, {newPos.x + 100, newPos.y + 100}));
    }

    // move all nodes
    ImVec2 offset = sourceRect.GetCenter();
    ImVec2 destCenter = destRect.GetCenter();
    offset.x -= destCenter.x;
    offset.y -= destCenter.y;
    for (auto& pos : nodePos)
    {
        pos.x += offset.x;
        pos.y += offset.y;
    }
    for (auto i = 0; i < m_nodes.size(); i++)
    {
        //SetNodePosition(i, nodePos[i]);
        m_nodes[i]->m_x = nodePos[i].x;
        m_nodes[i]->m_y = nodePos[i].y;
    }
}

size_t Graph::PickBestNode(const std::vector<NodeOrder>& orders)
{
    for (auto& order : orders)
    {
        if (order.mNodePriority == 0)
        {
            return order.mNodeIndex;
        }
    }
    // issue!
    assert(0);
    return -1;
}

void Graph::RecurseSetPriority(std::vector<NodeOrder>& orders,
                        const std::vector<Link>& links,
                        size_t currentIndex,
                        size_t currentPriority,
                        size_t& undeterminedNodeCount)
{
    if (!orders[currentIndex].mNodePriority)
    {
        undeterminedNodeCount--;
    }

    orders[currentIndex].mNodePriority = std::max(orders[currentIndex].mNodePriority, currentPriority + 1);
    for (auto& link : links)
    {
        if (link.m_outputNodeIndex == currentIndex)
        {
            RecurseSetPriority(orders, links, link.m_inputNodeIndex, currentPriority + 1, undeterminedNodeCount);
        }
    }
}

std::vector<Graph::NodeOrder> Graph::ComputeEvaluationOrders(const std::vector<Link>& links, size_t nodeCount)
{
    std::vector<NodeOrder> orders(nodeCount);
    for (size_t i = 0; i < nodeCount; i++)
    {
        orders[i].mNodeIndex = i;
        orders[i].mNodePriority = 0;
    }
    size_t undeterminedNodeCount = nodeCount;
    while (undeterminedNodeCount)
    {
        size_t currentIndex = PickBestNode(orders);
        RecurseSetPriority(orders, links, currentIndex, orders[currentIndex].mNodePriority, undeterminedNodeCount);
    };
    return orders;
}

std::vector<size_t> Graph::ComputeEvaluationOrder()
{
    std::vector<size_t> orderList;

    auto orders = ComputeEvaluationOrders(m_links, m_nodes.size());
    std::sort(orders.begin(), orders.end());
    orderList.resize(orders.size());
    for (size_t i = 0; i < orders.size(); i++)
    {
        orderList[i] = orders[i].mNodeIndex;
    }
    return orderList;
}
