#pragma once
#include <array>
#include <vector>
#include <ranges>
#include <map>
#include <any>
#include <utility>
#include <GraphLib/Common.hpp>

namespace GraphLib {
    template<class NodeAttrType = AnyAttrType, class EdgeAttrType = AnyAttrType>
    class SparseGraph {
      public:
        using NodeType = unsigned;
        using EdgeType = std::array<NodeType, 2>;
        using NodeListType = std::vector<NodeAttrType>;
        using AdjacencySubListType = std::vector<std::pair<NodeType, EdgeAttrType>>;
        using AdjacencyListType = std::vector<AdjacencySubListType>;

        struct NodesView {
            struct Iterator {
                using InnerIteratorType = NodeListType::iterator;
                using iterator_category = std::forward_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = NodeType;
                Iterator(auto inner_it) : _inner_it(inner_it), _inner_begin(inner_it) {};

                Iterator& operator++() {
                    ++_inner_it;
                    return *this;
                };

                value_type operator*() {
                    return _inner_it - _inner_begin;
                };
                friend bool operator==(const Iterator& a, const Iterator& b) {return a._inner_it == b._inner_it; };
                friend bool operator!=(const Iterator& a, const Iterator& b) { return !(a == b); };  

              private:
                InnerIteratorType _inner_begin;
                InnerIteratorType _inner_it;
            };

            NodesView(NodeListType& node_list) : _node_list(node_list) {};

            NodeAttrType& operator[](NodeType n) {
                return _node_list[n];
            };

            NodeAttrType& at(NodeType n) {
                return _node_list.at(n);
            };

            auto begin() {
                return Iterator(_node_list.begin());
            };

            auto end() {
                return Iterator(_node_list.end());
            };

          private:
            NodeListType& _node_list;
        };

        struct SubEdgesView {
            struct Iterator {
                using InnerIteratorType = AdjacencySubListType::iterator;
                using iterator_category = std::forward_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = EdgeType;
                Iterator(auto inner_it, NodeType& n) : _inner_it(inner_it), _n(n) {};

                Iterator& operator++() {
                    ++_inner_it;
                    return *this;
                };

                value_type operator*() {
                    return EdgeType{_n, _inner_it->first};
                };
                friend bool operator==(const Iterator& a, const Iterator& b) {return a._inner_it == b._inner_it; };
                friend bool operator!=(const Iterator& a, const Iterator& b) { return !(a == b); };  

              private:
                InnerIteratorType _inner_it;
                NodeType _n;
            };
            SubEdgesView(AdjacencyListType& adjacency_list, NodeType& n) : _adjacency_list(adjacency_list), _n(n) {};
            auto begin() {
                return Iterator(_adjacency_list[_n].begin(), _n);
            };

            auto end() {
                return Iterator(_adjacency_list[_n].end(), _n);
            };
          private:
            AdjacencyListType& _adjacency_list;
            NodeType _n;
        };

        struct EdgesView {
            struct Iterator {
                // Based on Ref.: https://stackoverflow.com/a/3623597/15141722
                // Copyright (c) 2010 James McNellis
                using InnerIteratorType = AdjacencySubListType::iterator;
                using OuterIteratorType = AdjacencyListType::iterator;
                using iterator_category = std::forward_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = EdgeType;
                using reference = EdgeType&;

                Iterator(auto outer_it, auto outer_end) : _outer_it(outer_it), _outer_end(outer_end) {
                    if (_outer_it != _outer_end) {
                        _inner_it = _outer_it->second.begin();
                    };
                };

                Iterator& operator++() {
                    while (_outer_it != _outer_end) {
                        ++_inner_it;
                        if(_inner_it == _outer_it->second.end()) {
                            ++_outer_it;
                            if (_outer_it != _outer_end) {
                                _inner_it = _outer_it->second.begin();
                            } else {
                                break;
                            };
                        };
                        if (_outer_it->first < _inner_it->first) {
                            break;
                        };
                    };
                    return *this;
                };

                value_type operator*() {
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
            };
            EdgesView(AdjacencyListType& adjacency_list) : _adjacency_list(adjacency_list) {};

            EdgeAttrType& operator[](EdgeType e) {
                return _adjacency_list[e[0]][e[1]];
            };

            EdgeAttrType& at(EdgeType e) {
                return _adjacency_list.at(e[0]),at(e[1]);
            };

            auto begin() {
                return Iterator(_adjacency_list.begin(), _adjacency_list.end());
            };

            auto end() {
                return Iterator(_adjacency_list.end(), _adjacency_list.end());
            };

          private:
            AdjacencyListType& _adjacency_list;
        };

        NodeType add_node(NodeAttrType node_attr={}) {
            _node_list.push_back(node_attr);
            _adjacency_list.push_back(AdjacencySubListType{});
            return number_of_nodes() - 1;
        };

        EdgeType add_edge(NodeType n1, NodeType n2, EdgeAttrType edge_attr={}) {
            _adjacency_list[n1].push_back(AdjacencySubListType{n2, edge_attr};
            _adjacency_list[n2].push_back(AdjacencySubListType{n1, edge_attr};
            return EdgeType{n1, n2};
        };

        EdgeType add_edge(EdgeType e, EdgeAttrType edge_attr={}) {
            add_edge(e[0], e[1], edge_attr);
            return e;
        };

        NodesView nodes() {
            return NodesView(_node_list);
        };

        auto neighbors(NodeType n) {
            return std::views::elements<0>(_adjacency_list.at(n));
        };

        EdgesView edges() {
            return EdgesView(_adjacency_list);
        };

        SubEdgesView edges(NodeType n) {
            return SubEdgesView(_adjacency_list, n);
        };

        unsigned number_of_nodes() {
            return _node_list.size();
        };

        unsigned number_of_edges() {
            auto it = edges();
            return std::distance(it.begin(), it.end());
        };

        void remove_node(NodeType n) {
            ListTools::del(_node_list, n);
            for (const auto &neighbor : neighbors(n)) {
                ListTools::del(_adjacency_list[neighbor], n);
            };
            ListTools::del(_adjacency_list, n);
        };

        void remove_edge(NodeType n1, NodeType n2) {
            ListTools::del(_adjacency_list[n1], n2);
            ListTools::del(_adjacency_list[n2], n1);
        };

        void remove_edge(EdgeType e) {
            remove_edge(e[0], e[1]);
        };

      private:
        NodeListType _node_list;
        AdjacencyListType _adjacency_list;
    };
};
