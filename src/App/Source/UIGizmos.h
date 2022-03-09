#pragma once
#include "Immath.h"
#include <debugdraw/debugdraw.h>
//#include "ImGuizmo.h"

class UIGizmos
{
public:
    UIGizmos(const float* view, const float* projection)
    : m_view(view)
    , m_projection(projection)
    {}
    
    void Edit(Imm::vec3* position, float* scale)
    {
        m_position = position;
        m_scale = scale;
        m_editing = true;
    }

    void Edit(Imm::vec3* position, Imm::vec3* orientation, float* scale)
    {
        m_position = position;
        m_orientation = orientation;
        m_scale = scale;
        m_editing = true;
    }
    
    void AddSphere(const Imm::matrix& matrix)
    {
        m_spheres.push_back(matrix);
    }

    // call once a frame when all UI have been visited
    void UI()
    {
        DebugSphapes();

        if (!m_editing)
        {
            return false;
        }

        Imm::matrix world;

        Imm::vec3 ori{0.f, 0.f, 0.f};
        if (m_orientation)
        {
            ori = *m_orientation;
        }
        Imm::vec3 sca{1.f, 1.f, 1.f};
        if (m_scale)
        {
            sca.x = *m_scale;
            sca.y = *m_scale;
            sca.z = *m_scale;
        }
        ImGuizmo::RecomposeMatrixFromComponents(&m_position->x, &ori.x, &sca.x, world.m16);
        EditTransform(m_view, m_projection, world.m16);

        ImGuizmo::DecomposeMatrixToComponents(world.m16, &m_position->x, &ori.x, &sca.x);
        
        *m_scale = sca.x;

        if (m_orientation)
        {
            *m_orientation = ori;
        }
    }
    
private:
    const float* m_view;
    const float* m_projection;
    bool m_editing{};
    Imm::vec3* m_position{};
    Imm::vec3* m_orientation{};
    float* m_scale{};

    std::vector<Imm::matrix> m_spheres;
    
    void DebugSphapes()
    {
        DebugDrawEncoder dde;

        bgfx::setViewTransform(0, m_view, m_projection);
        dde.begin(0);

        for (const auto& matrix : m_spheres)
        {
            dde.pushTransform(matrix.m16);
            dde.setColor(0xff0000ff);;
            dde.drawCircle({ 1.f,0.f,0.f }, { 0.f, 0.f, 0.f }, 1.f);
            dde.setColor(0xff00ff00);;
            dde.drawCircle({ 0.f,1.f,0.f }, { 0.f, 0.f, 0.f }, 1.f);
            dde.setColor(0xffff0000);;
            dde.drawCircle({ 0.f,0.f,1.f }, { 0.f, 0.f, 0.f }, 1.f);
            dde.popTransform();
        }

        dde.end();
        m_spheres.clear();
    }

    void EditTransform(const float* view, const float* projection, float* world)
    {
        static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(world, matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, world);

        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
        
        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::Manipulate(view, projection, ImGuizmo::UNIVERSAL, mCurrentGizmoMode, world, NULL, nullptr);
    }
};
