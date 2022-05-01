#pragma once
#include <array>
#include <map>
#include <ranges>
#include <iterator>
#include "Common.hpp"
#include "Views.hpp"

namespace GraphLib20 {
    template<class NodeType = unsigned, class NodeAttrType = AnyAttrType, class EdgeAttrType = AnyAttrType>
    class CustomMappedGraph {
      public:
        using EdgeType = std::array<NodeType, 2>;
        using NodeMapType = std::map<NodeType, NodeAttrType>;
        using AdjacencySubMapType = std::map<NodeType, EdgeAttrType>;
        using AdjacencyMapType = std::map<NodeType, AdjacencySubMapType>;
        using NodeViewType = MappedNodeView<NodeType, NodeAttrType>;
        using OutEdgeViewType = MappedOutEdgeView<NodeType, EdgeAttrType>;
        using EdgeViewType = MappedEdgeView<NodeType, EdgeAttrType>;

        NodeType add_node(NodeType n, NodeAttrType node_attr={}) {
            _node_map[n] = node_attr;
            _adjacency_map[n] = AdjacencySubMapType{};
            return n;
        };

        EdgeType add_edge(NodeType n1, NodeType n2, EdgeAttrType edge_attr={}) {
            _adjacency_map[n1][n2] = edge_attr;
            _adjacency_map[n2][n1] = edge_attr;
            return EdgeType{n1, n2};
        };

        EdgeType add_edge(EdgeType e, EdgeAttrType edge_attr={}) {
            add_edge(e[0], e[1], edge_attr);
            return e;
        };

        NodeViewType nodes() {
            return NodeViewType(_node_map);
        };

        auto neighbors(NodeType n) {
            return std::views::keys(_adjacency_map.at(n));
        };

        EdgeViewType edges() {
            return EdgeViewType(_adjacency_map);
        };

        OutEdgeViewType edges(NodeType n) {
            return OutEdgeViewType(_adjacency_map, n);
        };

        unsigned number_of_nodes() {
            return _node_map.size();
        };

        unsigned number_of_edges() {
            auto it = edges();
            return std::distance(it.begin(), it.end());
        };

        void remove_node(NodeType n) {
            for (const auto &neighbor : neighbors(n)) {
                MapTools::del(_adjacency_map[neighbor], n);
            };
            MapTools::del(_adjacency_map, n);
            MapTools::del(_node_map, n);
        };

        void remove_edge(NodeType n1, NodeType n2) {
            MapTools::del(_adjacency_map[n1], n2);
            MapTools::del(_adjacency_map[n2], n1);
        };

        void remove_edge(EdgeType e) {
            remove_edge(e[0], e[1]);
        };

      protected:
        NodeMapType _node_map;
        AdjacencyMapType _adjacency_map;
    };

    template<class NodeAttrType = AnyAttrType, class EdgeAttrType = AnyAttrType>
    class MappedGraph : public CustomMappedGraph<unsigned, NodeAttrType, EdgeAttrType> {
      public:
        using AdjacencySubMapType = std::map<unsigned, EdgeAttrType>;

        unsigned add_node(NodeAttrType node_attr={}) {
            unsigned n = this->number_of_nodes();
            this->_node_map[n] = node_attr;
            this->_adjacency_map[n] = AdjacencySubMapType{};
            return n;
        };

        void remove_node(unsigned n) {
            for (const auto &other : this->nodes()) {
                MapTools::del_and_shift(this->_adjacency_map[other], n);
            };
            MapTools::del_and_shift(this->_adjacency_map, n);
            MapTools::del_and_shift(this->_node_map, n);
        };
    };
};
