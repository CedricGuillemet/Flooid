#pragma once
#include "Immath.h"

class Camera
{
public:
    Camera(bool homogeneousDepth, float alpha = 0.f, float beta = 0.f) :
        m_homogeneousDepth(homogeneousDepth)
        , m_alpha(alpha)
        , m_beta(beta)
        , m_alphaTarget(alpha)
        , m_betaTarget(beta)
        , m_radius(2.5f)
        , m_width(1)
        , m_height(1)
    {
        ComputeMatrices();
    }
    void SetDisplaySize(uint16_t width, uint16_t height);
    void Input(float dx, float dy);
    Imm::matrix GetViewProjection();
private:
    void ComputeMatrices();
    Imm::matrix m_viewMatrix;
    Imm::matrix m_projectionMatrix;
    bool m_homogeneousDepth;
    float m_alpha;
    float m_beta;
    float m_alphaTarget;
    float m_betaTarget;
    float m_radius;
    uint16_t m_width;
    uint16_t m_height;
};
