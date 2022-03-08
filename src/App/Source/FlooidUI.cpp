#include "FlooidUI.h"
#include "imgui.h"
#include "UIGizmos.h"
#include "Camera.h"

FlooidUI::FlooidUI(Graph& graph)
: m_graphEditorDelegate(graph)
, m_graph(graph)
{
}

void FlooidUI::CheckUsingUI()
{
    ImGuiIO& io = ImGui::GetIO();
    
    if (ImGui::IsItemHovered() || ImGui::IsWindowHovered())
    {
        m_usingGUI = true;
    }
    else
    {
        if (m_usingGUI && !io.MouseDown[0] && !io.MouseDown[1] && !io.MouseDown[2])
        {
            m_usingGUI = false;
        }
    }
}

bool FlooidUI::GraphUI()
{
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);

    ImGui::Begin("Graph Editor", NULL, 0);
    if (ImGui::Button("Fit all nodes"))
    {
        m_graphEditorFit = GraphEditor::Fit_AllNodes;
    }
    ImGui::SameLine();
    if (ImGui::Button("Fit selected nodes"))
    {
        m_graphEditorFit = GraphEditor::Fit_SelectedNodes;
    }
    GraphEditor::Show(m_graphEditorDelegate, m_graphEditorOptions, m_graphEditorViewState, true, &m_graphEditorFit);
    CheckUsingUI();
    ImGui::End();
    return m_usingGUI;
}

bool FlooidUI::ParametersUI(const Camera& camera)
{
    ImGui::SetNextWindowPos(ImVec2(0.0f, 300.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_FirstUseEver);

    ImGui::Begin("Parameters", NULL, 0);
    auto selectedNode = m_graph.GetSelectedNode();
    if (selectedNode)
    {
        UIGizmos uiGizmos(camera.GetView().m16, camera.GetProjection().m16);
        selectedNode->UI(uiGizmos);
        m_usingGUI |= uiGizmos.UI();
    }
    CheckUsingUI();
    ImGui::End();
    return m_usingGUI;
}

bool FlooidUI::UI(const Camera& camera)
{
    bool usingUI = GraphUI();
    usingUI |= ParametersUI(camera);
    return usingUI;
}

