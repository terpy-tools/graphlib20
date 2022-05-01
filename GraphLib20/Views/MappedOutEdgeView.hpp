#pragma once
#include <cstddef>
#include <array>
#include <map>
#include <iterator>

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
