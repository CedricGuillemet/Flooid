#include "FlooidUI.h"
#include "imgui.h"
#include "UIGizmos.h"
#include "Camera.h"

FlooidUI::FlooidUI(Graph& graph)
: m_graphEditorDelegate(graph)
, m_graph(graph)
{
}

void FlooidUI::GraphUI()
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
    ImGui::SameLine();
    if (ImGui::Button("Layout"))
    {
        auto nodeOrder = m_graph.ComputeEvaluationOrder(m_graph.m_links, m_graph.GetNodes().size());
        printf("");
    }
    ImGui::SameLine();
    if (ImGui::Button("Options"))
    {
        m_graph.UnselectAll();
        m_editingOptions = true;
    }
    ImGui::SameLine();
    if (ImGui::Button(m_running?"Pause":"Run"))
    {
        m_running = !m_running;
    }

    GraphEditor::Show(m_graphEditorDelegate, m_graphEditorOptions, m_graphEditorViewState, true, &m_graphEditorFit);
    
    ImGui::End();
}

void FlooidUI::ParametersUI(const Camera& camera)
{
    ImGui::SetNextWindowPos(ImVec2(0.0f, 300.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_FirstUseEver);

    ImGui::Begin("Parameters", NULL, 0);
    auto selectedNode = m_graph.GetSelectedNode();
    if (selectedNode)
    {
        m_editingOptions = false;
        UIGizmos uiGizmos(camera.GetView().m16, camera.GetProjection().m16);
        selectedNode->UI(uiGizmos);
        uiGizmos.UI();
    }
    else if (m_editingOptions)
    {
        GraphEditor::EditOptions(m_graphEditorOptions);
    }
    ImGui::End();
}

void FlooidUI::UI(const Camera& camera)
{
    GraphUI();
    ParametersUI(camera);
}

