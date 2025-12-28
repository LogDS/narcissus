// Field.h
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

#ifndef NARCISSUS_FIELD_H
#define NARCISSUS_FIELD_H

#include <any>
#include <cinttypes>
#include <functional>
#include <string_view>
#include <typeindex>
#include <memory>

#include <narcissus/reflection/type_cases.h>

class Reflection;

class   Field {
    uint64_t idx, size_;
    std::string name;
    std::function<std::any(const std::any&)> getobj;
    type_cases case_;
    uint64_t bounded_array_size;
    std::type_index val_t;

public:
    Field(const std::type_index& val_t, uint64_t val, const std::string& name, std::function<std::any(const std::any&)> getter, type_cases cases_ = T_UNEXPECTED, uint64_t bounded_array_size = 0, uint64_t size_ = 0) : val_t{val_t},
    idx{val}, size_(size_), name{name}, getobj(getter), case_(cases_), bounded_array_size(bounded_array_size) {}

    virtual ~Field() = default;

    virtual Reflection* asReflection() const;

    uint64_t index() const {
        return idx;
    }
    virtual std::any any_value(const std::any&x) const {
        return getobj(x);
    }
    template <typename K> K value(const std::any&x) const {
        return std::any_cast<K>(any_value(x));
    }
    const std::string_view get_field_name() const {
        return name;
    }
    type_cases type() const {
        return case_;
    }
    uint64_t size_type() const {
        return size_;
    }
    uint64_t size_if_bounded_array() const {
        return bounded_array_size;
    }
};


#endif //NARCISSUS_FIELD_H