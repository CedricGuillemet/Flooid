#pragma once
#include "GraphNode.h"
#include "GraphEditorDelegate.h"

class Camera;

class FlooidUI
{
public:
    FlooidUI(Graph& graph);
    
    void UI(const Camera& camera);
    
    bool Running() const { return m_running; }
    
    void GraphFitAllNodes() { m_graphEditorFit = GraphEditor::Fit_AllNodes; }
private:
    void GraphUI();
    void ParametersUI(const Camera& camera);
    
    GraphEditor::Options m_graphEditorOptions;
    GraphEditorDelegate m_graphEditorDelegate;
    GraphEditor::ViewState m_graphEditorViewState{{0.0f, 0.0f}, 0.5f, 0.5f};
    GraphEditor::FitOnScreen m_graphEditorFit{ GraphEditor::Fit_None };
    Graph& m_graph;
    
    bool m_editingOptions{false};
    bool m_running{true};
};
