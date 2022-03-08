#include "Camera.h"

Imm::matrix Camera::GetViewProjection() 
{ 
    return m_viewMatrix * m_projectionMatrix; 
}

void Camera::Input(float dx, float dy)
{
    m_alphaTarget += dx * 10.f;
    m_betaTarget += dy * 10.f;
    m_betaTarget = Imm::Clamp(m_betaTarget, 0.001f, 1.57f);
}

void Camera::ComputeMatrices()
{
    m_alpha = Imm::Lerp(m_alpha, m_alphaTarget, 0.2f);
    m_beta = Imm::Lerp(m_beta, m_betaTarget, 0.2f);
    Imm::vec4 target{0.f, 1.f, 0.f, 0.f};
    Imm::vec4 eye = target;
    eye += Imm::vec4(sinf(m_alpha), 0.f, cosf(m_alpha), 0.f) * m_radius * cosf(m_beta);
    eye.y += sinf(m_beta) * m_radius;
    m_viewMatrix.lookAtLH(eye, target, { 0.f, 1.f, 0.f, 0.f });
    m_projectionMatrix.glhPerspectivef2(54.f, float(m_width)/float(m_height), 0.1f, 100.f, m_homogeneousDepth);
}

void Camera::SetDisplaySize(uint16_t width, uint16_t height)
{
    m_width = width;
    m_height = height;
}
