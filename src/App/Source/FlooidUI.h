#pragma once
#include "GraphNode.h"
#include "GraphEditorDelegate.h"

class FlooidUI
{
public:
    FlooidUI(Graph& graph);
    
    bool UI();
    
private:
    void CheckUsingUI();
    bool m_usingGUI{false};

    bool GraphUI();
    bool ParametersUI();
    
    GraphEditor::Options m_graphEditorOptions;
    GraphEditorDelegate m_graphEditorDelegate;
    GraphEditor::ViewState m_graphEditorViewState{{0.0f, 0.0f}, { 0.5f },{ 0.5f }};
    GraphEditor::FitOnScreen m_graphEditorFit{ GraphEditor::Fit_None };

};
