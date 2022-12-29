#include "core/dotoutput.h"

#include <iostream>

#include "core/stdutils.h"



namespace euphoria::core::dot
{
    // std::vector<Node> nodes;
    // std::vector<Edge> edges;

    Node* Graph::add_new_node(const std::string& name)
    {
        const auto id = nodes.size();
        nodes.emplace_back();

        Node* r = &nodes[id];
        r->id = static_cast<NodeId>(id);
        r->label = name;

        return r;
    }


    Edge*
    Graph::add_new_edge(NodeId from, NodeId to)
    {
        const auto id = edges.size();
        edges.emplace_back();
        
        Edge* r = &edges[id];
        r->from = from;
        r->to = to;

        return r;
    }

    std::string
    id_to_string(NodeId id)
    {
        return "node_{}"_format(core::base_cast(id));
    }
    
    void
    Graph::print()
    {
        constexpr const auto* const i = "    ";
        auto& out = std::cout;

        out << "digraph G\n";
        out << "{\n";
        for(const auto& node: nodes)
        {
            // todo(Gustav): escape label
            out << i << id_to_string(node.id) << "[" << "label=\"" << node.label << "\"];\n";
        }

        // todo(Gustav): sort edges?
        for(const auto& e: edges)
        {
            out << i << id_to_string(e.from) << " -> " << id_to_string(e.to) << ";\n";
        }
        out << "}\n";
    }
}
