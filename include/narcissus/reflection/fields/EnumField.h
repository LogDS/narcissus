// EnumField.h
// This file is part of narcissus
//
// Copyright (C)  2025 - gyankos
//
// narcissus is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  narcissus is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with narcissus. If not, see <http://www.gnu.org/licenses/>.

//
// Created by gyankos on 27/12/25.
//

#ifndef NARCISSUS_ENUMFIELD_H
#define NARCISSUS_ENUMFIELD_H

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <narcissus/reflection/fields/Field.h>

class EnumField : public Field {
    std::vector<std::string> case_names;
    std::vector<long long> case_to_underlying_type;
    std::unordered_map<long long, uint64_t> underlying_type_to_index;
    std::unordered_map<std::string, uint64_t> case_name_to_index;
    std::function<int64_t(const std::any&)> to_underlying;
public:

    EnumField(const std::type_index& val_t, uint64_t val,
        const std::string& name,
        const std::vector<std::pair<std::string, long long>>& values,
        std::function<std::any(const std::any&)> getter,
        std::function<int64_t(const std::any&)>&& to_underlying,
        type_cases cases_ = T_UNEXPECTED) : Field(val_t, val, name, (getter), cases_, values.size(),  0), to_underlying{std::move(to_underlying)} {
        for (uint64_t idx = 0, N = values.size(); idx < N; ++idx) {
            case_name_to_index[values[idx].first] = idx;
            case_names.emplace_back(values[idx].first);
            case_to_underlying_type.emplace_back(values[idx].second);
            underlying_type_to_index[values[idx].second] = idx;
        }
    }

    uint64_t size() {
        return case_names.size();
    }

    /*template <typename T>*/ uint64_t toIndex(std::any value) const {
        // static_assert(std::is_enum_v<T>);
        int64_t val = to_underlying(value);
        auto it = underlying_type_to_index.find(val);
        if (it != underlying_type_to_index.end())
            return it->second;
        return -1;
    }

    /*template <typename T> T fromIndex(uint64_t index) const {
        static_assert(std::is_enum_v<T>);
        return case_to_underlying_type.at(index);
    }*/

    /**
     * Given a raw value associated with the original type, this returns the string representation associated with the enumerated value
     */
    /*template <typename T>*/ std::string toString(std::any value) const {
        // static_assert(std::is_enum_v<T>);
        // long long val = static_cast<long long>(std::to_underlying(value));
        int64_t val = to_underlying(value);
        auto it = underlying_type_to_index.find(val);
        if (it != underlying_type_to_index.end())
            return std::string(case_names[it->second]);
        return "";
    }

    const std::string_view str(uint64_t index) {
        return case_names.at(index);
    }
};


#endif //NARCISSUS_ENUMFIELD_H