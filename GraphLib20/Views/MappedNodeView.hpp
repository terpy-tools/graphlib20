#pragma once
#include <map>
#include <ranges>

template<class NodeType, class NodeAttrType>
class MappedNodeView {
  public:
    using NodeMapType = std::map<NodeType, NodeAttrType>;

    MappedNodeView(NodeMapType& node_map) : _node_map(node_map) {};

    NodeAttrType& operator[](NodeType n) {
        return _node_map[n];
    };

    NodeAttrType& at(NodeType n) {
        return _node_map.at(n);
    };

    auto begin() {
        return std::views::keys(_node_map).begin();
    };

    auto end() {
        return std::views::keys(_node_map).end();
    };

  private:
    NodeMapType& _node_map;
};
