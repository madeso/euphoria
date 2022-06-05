#pragma once





namespace euphoria::core::dot
{


enum class NodeId : int {};

struct Node
{
    NodeId id;
    std::string label;
};

struct Edge
{
    NodeId from;
    NodeId to;
};

struct Graph
{
    std::vector<Node> nodes;
    std::vector<Edge> edges;

    Node* add_new_node(const std::string& name);
    Edge* add_new_edge(NodeId from, NodeId to);

    void print();
};


template
<
    typename T,
    typename LabelFunction,
    typename FindChildrenFunction
>
NodeId
fill_graph
(
    const T& item,
    LabelFunction label_function,
    FindChildrenFunction find_children_function,
    Graph* graph
)
{
    const auto label = label_function(item);
    const auto self_node = graph->add_new_node(label)->id;

    const auto children = find_children_function(item);
    for (auto it = children.begin(); it != children.end(); ++it)
    {
        const auto child_node = fill_graph<T, LabelFunction, FindChildrenFunction>
        (
            *it,
            label_function,
            find_children_function,
            graph
        );

        graph->add_new_edge(self_node, child_node);
    }

    return self_node;
}


template
<
    typename T,
    typename LabelFunction,
    typename FindChildrenFunction
>
Graph
create_graph
(
    const T& item,
    LabelFunction label_function,
    FindChildrenFunction find_children_function
)
{
    Graph graph;
    
    fill_graph<T, LabelFunction, FindChildrenFunction>
    (
        item,
        label_function,
        find_children_function,
        &graph
    );

    return graph;
}

}
