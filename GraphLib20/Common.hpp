#pragma once
#include <any>
#include <map>
#include <string>
#include <iterator>
#include <algorithm>
#include <utility>
#include <stdexcept>
#include <source_location>

inline const char* print_function_name(const std::source_location& location = std::source_location::current()) {
    // Returns the name of the caller
    return location.function_name();
};

namespace GraphLib20 {
    struct NoAttr {};
    using AnyAttrType = std::map<std::string, std::any>;
    using EnumAnyAttrType = std::map<int, std::any>;
};

namespace GraphLib20::MapTools {
    static const size_t npos = -1;

    template<template <class, class> class MapType, class KeyType, class ValType>
    ValType get(MapType<KeyType, ValType>& map, unsigned idx) {
        if (idx >= map.size()) {
            throw std::out_of_range(print_function_name());
        };
        auto it = map.begin();
        std::advance(it, idx);
        return it->second;
    };

    template<class MapType, class KeyType>
    void del(MapType& map, KeyType key) {
        auto it = map.find(key);
        if (it != map.end()) {
            map.erase(it);
        };
    };

    template<class MapType, class KeyType>
    void del_and_shift(MapType& map, KeyType key) {
        bool key_found = false;
        auto it = map.begin();
        // Find and remove the key
        for (it; it != map.end(); ++it) {
            if (it->first >= key) {
                if (it->first == key) {
                    it = map.erase(it);
                    key_found = true;
                };
                break;
            };
        };
        // Decrement all subsequent keys
        for (it; it != map.end(); ++it) {
            auto handle = map.extract(it->first);
            handle.key() -= KeyType(1);
            map.insert(std::move(handle));
        };
    };

    template<class MapType, class KeyType>
    size_t index(MapType& map, KeyType key) {
        auto it = map.find(key);
        if (it != map.end()) {
            return std::distance(map.begin(), it);
        } else {
            return npos;
        };
    };
};

namespace GraphLib20::ListTools {
    static const size_t npos = -1;

    template<class ListType>
    void del(ListType& list, unsigned idx) {
        list.erase(list.begin() + idx);
    };

    template<class ListType, class ElementType>
    size_t index(const ListType& list, ElementType element) {
        auto it = std::find(list.begin(), list.end(), element);
        if (it != list.end()) {
            return std::distance(list.begin(), it);
        } else {
            return npos;
        };
    };
};

namespace GraphLib20::MatTools {
    template<class MatType>
    void remove_idx(const MatType& mat, unsigned idx) {
        // Based on Ref.: https://stackoverflow.com/a/46303314
        // Copyright (c) 2014 Andrew
        // Copyright (c) 2019 raahlb
        unsigned int num_rows = mat.rows() - 1;
        unsigned int num_cols = mat.cols();
        if (idx < num_rows) {
            mat.block(idx, 0, num_rows-idx, num_cols) = mat.bottomRows(num_rows-idx);
        };
        num_cols -= 1;
        if (idx < num_cols) {
            mat.block(0, idx, num_rows, num_cols-idx) = mat.rightCols(num_cols-idx);
        };
        mat.conservativeResize(num_rows, num_cols);
    };
};
