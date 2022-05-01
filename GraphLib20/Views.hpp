#pragma once
#include <cstddef>
#include <array>
#include <map>
#include <iterator>
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

template<class NodeType, class EdgeAttrType>
class MappedOutEdgeView {
  public:
    using EdgeType = std::array<NodeType, 2>;
    using AdjacencySubMapType = std::map<NodeType, EdgeAttrType>;
    using AdjacencyMapType = std::map<NodeType, AdjacencySubMapType>;

    struct Iterator {
        using InnerIteratorType = AdjacencySubMapType::iterator;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = EdgeType;

        Iterator(auto inner_it, NodeType& n) : _inner_it(inner_it), _n(n) {};

        Iterator& operator++() {
            ++_inner_it;
            return *this;
        };
        Iterator operator++(int) { Iterator it = *this; ++(*this); return it; };
        EdgeType operator*() {
            return EdgeType{_n, _inner_it->first};
        };
        friend bool operator==(const Iterator& a, const Iterator& b) {return a._inner_it == b._inner_it; };
        friend bool operator!=(const Iterator& a, const Iterator& b) { return !(a == b); };  

      private:
        InnerIteratorType _inner_it;
        NodeType _n;
    };
    MappedOutEdgeView(AdjacencyMapType& adjacency_map, NodeType& n) : _adjacency_map(adjacency_map), _n(n) {};
    auto begin() {
        return Iterator(_adjacency_map[_n].begin(), _n);
    };

    auto end() {
        return Iterator(_adjacency_map[_n].end(), _n);
    };
  private:
    AdjacencyMapType& _adjacency_map;
    NodeType _n;
};

template<class NodeType, class EdgeAttrType>
class MappedEdgeView {
  public:
    using EdgeType = std::array<NodeType, 2>;
    using AdjacencySubMapType = std::map<NodeType, EdgeAttrType>;
    using AdjacencyMapType = std::map<NodeType, AdjacencySubMapType>;

    struct Iterator {
        // Based on Ref.: https://stackoverflow.com/a/3623597/15141722
        // Copyright (c) 2010 James McNellis
        using InnerIteratorType = AdjacencySubMapType::iterator;
        using OuterIteratorType = AdjacencyMapType::iterator;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = EdgeType;

        Iterator(auto outer_it, auto outer_end) : _outer_it(outer_it), _outer_end(outer_end) {
            if (_outer_it != _outer_end) {
                _inner_it = _outer_it->second.begin();
                advance_past_empty_inner_containers();
            };
        };

        Iterator& operator++() {
            do {
                ++_inner_it;
                advance_past_empty_inner_containers();
            } while (_outer_it != _outer_end && _outer_it->first > _inner_it->first);
            return *this;
        };
        Iterator operator++(int) { Iterator it = *this; ++(*this); return it; };
        EdgeType operator*() {
            return EdgeType{_outer_it->first, _inner_it->first};
        };
        friend bool operator==(const Iterator& a, const Iterator& b) {
            if (a._outer_it != b._outer_it) {
                return false;
            };
            if (a._outer_it != a._outer_end && b._outer_it != b._outer_end && a._inner_it != b._inner_it) {
                return false;
            };
            return true;
        };
        friend bool operator!=(const Iterator& a, const Iterator& b) { return !(a == b); };  

      private:
        OuterIteratorType _outer_end;
        OuterIteratorType _outer_it;
        InnerIteratorType _inner_it;

        void advance_past_empty_inner_containers() {
            while(_outer_it != _outer_end && _inner_it == _outer_it->second.end()) {
                ++_outer_it;
                if (_outer_it != _outer_end) {
                    _inner_it = _outer_it->second.begin();
                };
            };
        };
    };
    MappedEdgeView(AdjacencyMapType& adjacency_map) : _adjacency_map(adjacency_map) {};

    EdgeAttrType& operator[](EdgeType e) {
        return _adjacency_map[e[0]][e[1]];
    };
    EdgeAttrType& at(EdgeType e) {
        return _adjacency_map.at(e[0]),at(e[1]);
    };

    auto begin() {
        return Iterator(_adjacency_map.begin(), _adjacency_map.end());
    };

    auto end() {
        return Iterator(_adjacency_map.end(), _adjacency_map.end());
    };

  private:
    AdjacencyMapType& _adjacency_map;
};
