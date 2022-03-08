#pragma once
#include "Immath.h"
//#include "ImGuizmo.h"

class UIGizmos
{
public:
    UIGizmos(const float* view, const float* projection)
    : m_view(view)
    , m_projection(projection)
    , m_editing{}
    {}
    
    void EditPositionScale(Imm::vec3* position, float* scale)
    {
        m_position = position;
        m_scale = scale;
        m_editing = true;
    }
    
    void UI()
    {
        if (!m_editing)
        {
            return;
        }
        
        Imm::matrix world;
        world.translationScale({m_position->x, m_position->y, m_position->z, 0.f}, {*m_scale, *m_scale, *m_scale, 0.f});
        EditTransform(m_view, m_projection, world.m16);
        
        m_position->x = world.position.x;
        m_position->y = world.position.y;
        m_position->z = world.position.z;
        
        *m_scale = world.right.length();
    }
    
private:
    const float* m_view;
    const float* m_projection;
    bool m_editing{};
    Imm::vec3* m_position{};
    float* m_scale;
    
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
