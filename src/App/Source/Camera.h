#pragma once
#include "Immath.h"

class Camera
{
public:
    Camera()
    {
        m_viewMatrix.lookAtLH({5.f, 5.f, 5.f, 0.f}, {0.f, 0.f, 0.f, 0.f}, {0.f, 1.f, 0.f, 0.f});
        m_projectionMatrix.glhPerspectivef2(54.f, 1.f, 0.1f, 100.f);
    }

    Imm::matrix GetViewProjection() const { return m_viewMatrix * m_projectionMatrix; }
    Imm::matrix m_viewMatrix;
    Imm::matrix m_projectionMatrix;
};
