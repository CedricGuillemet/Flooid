#include "FlooidUI.h"
#include "imgui.h"

FlooidUI::FlooidUI(Graph& graph)
: m_graphEditorDelegate(graph)
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

bool FlooidUI::ParametersUI()
{
    ImGui::SetNextWindowPos(ImVec2(0.0f, 300.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_FirstUseEver);

    ImGui::Begin("Parameters", NULL, 0);
    CheckUsingUI();
    ImGui::End();
    return false;
}

bool FlooidUI::UI()
{
    bool usingUI = GraphUI();
    usingUI |= ParametersUI();
    return usingUI;
}

