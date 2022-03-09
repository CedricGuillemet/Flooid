#pragma once
#include "GraphEditor.h"
#include "GraphNode.h"

struct GraphEditorDelegate : public GraphEditor::Delegate
{
    GraphEditorDelegate(Graph& graph) : m_graph(graph)
    {
    }
    
    bool AllowedLink(GraphEditor::NodeIndex from, GraphEditor::NodeIndex to) override
    {
        return true;
    }

    void SelectNode(GraphEditor::NodeIndex nodeIndex, bool selected) override
    {
        m_graph.GetNodes()[nodeIndex]->m_selected = selected;
    }

    void MoveSelectedNodes(const ImVec2 delta) override
    {
        for (auto node : m_graph.GetNodes())
        {
            if (!node->m_selected)
            {
                continue;
            }
            node->m_x += delta.x;
            node->m_y += delta.y;
        }
    }

    virtual void RightClick(GraphEditor::NodeIndex nodeIndex, GraphEditor::SlotIndex slotIndexInput, GraphEditor::SlotIndex slotIndexOutput) override
    {
    }

    void AddLink(GraphEditor::NodeIndex inputNodeIndex, GraphEditor::SlotIndex inputSlotIndex, GraphEditor::NodeIndex outputNodeIndex, GraphEditor::SlotIndex outputSlotIndex) override
    {
        m_graph.AddLink({ static_cast<uint32_t>(inputNodeIndex),
                          static_cast<uint8_t>(inputSlotIndex),
                          static_cast<uint32_t>(outputNodeIndex),
                          static_cast<uint8_t>(outputSlotIndex) });
    }

    void DelLink(GraphEditor::LinkIndex linkIndex) override
    {
        m_graph.EraseLink(linkIndex);
    }

    void CustomDraw(ImDrawList* drawList, ImRect rectangle, GraphEditor::NodeIndex nodeIndex) override
    {
    }

    const size_t GetTemplateCount() override
    {
        return mTemplateFunctions.size();
    }

    const GraphEditor::Template GetTemplate(GraphEditor::TemplateIndex index) override
    {
        return mTemplateFunctions[index]();
    }

    const size_t GetNodeCount() override
    {
        auto nodes = m_graph.GetNodes();
        return nodes.size();
    }

    const GraphEditor::Node GetNode(GraphEditor::NodeIndex index) override
    {
        auto nodes = m_graph.GetNodes();
        const auto myNode = nodes[index];
        return GraphEditor::Node
        {
            myNode->GetName(),
            myNode->GetRuntimeType(),
            ImRect(ImVec2(myNode->m_x, myNode->m_y), ImVec2(myNode->m_x + 200, myNode->m_y + 100)),
            myNode->m_selected
        };
    }

    const size_t GetLinkCount() override
    {
        return m_graph.GetLinks().size();
    }

    const GraphEditor::Link GetLink(GraphEditor::LinkIndex index) override
    {
        auto link = m_graph.GetLinks()[index];
        return {link.m_InputNodeIndex,
                link.m_InputSlotIndex,
                link.m_OutputNodeIndex,
                link.m_OutputSlotIndex};
    }

    typedef GraphEditor::Template (*TemplateFunction)();
    static inline std::vector<TemplateFunction> mTemplateFunctions;
protected:
    Graph& m_graph;
};
