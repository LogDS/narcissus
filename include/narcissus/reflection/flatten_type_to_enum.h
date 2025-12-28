// flatten_type_to_enum.h
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

#ifndef NARCISSUS_FLATTEN_TYPE_TO_ENUM_H
#define NARCISSUS_FLATTEN_TYPE_TO_ENUM_H

#include <field_reflection.hpp>
#include <magic_enum/magic_enum.hpp>
#include <narcissus/reflection/fields/EnumField.h>
#include <narcissus/reflection/fields/PtrField.h>
#include <narcissus/reflection/fields/ArrayField.h>
#include <narcissus/reflection/fields/DynamicArrayField.h>
#include <narcissus/reflection/fields/VariantField.h>

#include "ReflectionManager.h"
#include <iostream>

template <typename T> std::unique_ptr<Field> flatten_type_to_enum(uint64_t val, std::string name, std::function<std::any(const std::any&)> getter) {
    std::type_index type_i = std::type_index(typeid(T));
    if constexpr (std::is_same_v<T, std::string>) {
        return std::make_unique<Field>(type_i, val, name, getter, T_STRING, 0, 0);
    }
    if constexpr (std::is_void_v<T>) {
        return std::make_unique<Field>(type_i, val, name, getter, T_VOID, 0, 0);
    }
    else if constexpr (std::is_null_pointer_v<T>) {
        return  std::make_unique<Field>(type_i, val, name, getter,T_NULLPTR, 0, 0);
    }
    else if constexpr (std::is_integral_v<T>) {
        if constexpr (std::is_signed_v<T>) {
            return  std::make_unique<Field>(type_i, val, name, getter,  T_SIGNED_INTEGRAL, 0, sizeof(T));
        } else {
            return  std::make_unique<Field>(type_i, val, name, getter,  T_U_INTEGRAL, 0, sizeof(T));
        }
    }
    else if constexpr (std::is_floating_point_v<T>) {
        if constexpr (std::is_signed_v<T>) {
            return  std::make_unique<Field>(type_i, val, name, getter,  T_SIGNED_FLOAT, 0, sizeof(T));
        } else {
            return  std::make_unique<Field>(type_i, val, name, getter,  T_U_FLOAT, 0, sizeof(T));
        }
    }
    else if constexpr (is_std_array<T>::value) {
        std::function<std::unique_ptr<Field>(const uint64_t)> cell_getter = [val,name,getter](const uint64_t idx) {
            return flatten_type_to_enum<remove_arg<T>>(val, name+"["+std::to_string(idx)+"]", [getter,idx](const std::any& val) {
                return std::any_cast<T>(getter(val))[idx];
            });
        };
        return  std::make_unique<ArrayField>(type_i, std::move(cell_getter), val, name, getter,T_STATIC_ARRAY, std_array_size<T>::size, 0);
    }
    else if constexpr (is_vector<T>::value) {
        std::function<uint64_t(const std::any& )> size_getter = [getter](const std::any& obj) {
            return std::any_cast<T>(getter(obj)).size();
        };
        std::function<std::unique_ptr<Field>(const uint64_t)> cell_getter = [val,name,getter](const uint64_t idx) {
            return flatten_type_to_enum<remove_arg<T>>(val, name+"["+std::to_string(idx)+"]", [getter,idx](const std::any& val) {
                return std::any_cast<T>(getter(val))[idx];
            });
        };
        return  std::make_unique<DynamicArrayField>(type_i, std::move(cell_getter), std::move(size_getter), val, name, getter,T_OTHER_ARRAY, -1, 0);
    }
    else if constexpr (is_list<T>::value) {
        std::function<uint64_t(const std::any& )> size_getter = [getter](const std::any& obj) {
            return std::any_cast<T>(getter(obj)).size();
        };
        std::function<std::unique_ptr<Field>(const uint64_t)> cell_getter = [val,name,getter](const uint64_t idx) {
            return flatten_type_to_enum<remove_arg<T>>(val, name+"["+std::to_string(idx)+"]", [getter,idx](const std::any& val) {
                auto it = std::next(std::any_cast<T>(getter(val)).begin(), idx);
                // std::advance(it, idx);
                return *it;
            });
        };
        return  std::make_unique<DynamicArrayField>(type_i, std::move(cell_getter), std::move(size_getter), val, name, getter,T_OTHER_ARRAY, -1, 0);
    }
    else if constexpr (std::is_array_v<T>) {
        // UNfortunately, this is not currently supported by the library,
        if constexpr (std::is_bounded_array_v<T>) {
            return  std::make_unique<Field>(type_i, val, name, getter, T_STATIC_ARRAY, std::extent_v<T>, std::remove_reference<decltype( std::declval<T>()[0] )>::type);
        } else {
            return  std::make_unique<Field>(type_i, val, name, getter, T_OTHER_ARRAY, -1, std::remove_reference<decltype( std::declval<T>()[0] )>::type);
        }
    }
    else if constexpr (std::is_enum_v<T>) {
        auto color_entries = magic_enum::enum_entries<T>();
        std::vector<std::pair<std::string, long long>> tmp_vals;
        assert(MAGIC_ENUM_RANGE_MIN <= std::to_underlying(std::numeric_limits<T>::min()));
        assert(MAGIC_ENUM_RANGE_MAX >= std::to_underlying(std::numeric_limits<T>::min()));
        tmp_vals.reserve(color_entries.size());
        for (const auto& [enum_val, enum_string] : color_entries) {
            tmp_vals.emplace_back(std::string(enum_string), std::to_underlying(enum_val));
        }
        std::function<int64_t(const std::any&)> underlying = [](const std::any& val) {
          return std::to_underlying(std::any_cast<T>(val));
        };
        return std::make_unique<EnumField>(type_i, 0, name, tmp_vals, (getter), std::move(underlying), T_ENUM);
        // return  nullptr; //EnumField::from_type<T>((getter));
    }
    else if constexpr (is_tuple<T>::value) {
        ReflectionManager::reflection_record_create<T>();
        return  std::make_unique<Field>(type_i, val, name, getter, T_TUPLE, 0, 0);
    }
    else if constexpr (std::is_union_v<T>) {
        return  std::make_unique<Field>(type_i, val, name, getter,T_UNION, 0, 0);
    }
    else if constexpr (is_smart_pointer<T>::value) {
        if constexpr (is_shared<T>::value) {
            std::function<std::any(const std::any&)> neu_getter = [getter](const std::any&val) {
                return std::any_cast<T>(getter(val)).get();
            };
            std::unique_ptr<Field> original_field = flatten_type_to_enum<typename remove_arg<T>::type>(val, name, [neu_getter](const std::any& val) { return *std::any_cast<T*>(neu_getter(val));});
            return  std::make_unique<PtrField>(type_i, std::move(original_field), val, name, neu_getter,T_POINTER, 0, 0);
        } else if constexpr (is_weak<T>::value) {
            std::function<std::any(const std::any&)> neu_getter = [getter](const std::any&val) {
                return std::any_cast<T>(getter(val)).get();
            };
            std::unique_ptr<Field> original_field = flatten_type_to_enum<typename remove_arg<T>::type>(val, name, [neu_getter](const std::any& val) { return *std::any_cast<T*>(neu_getter(val));});
            return  std::make_unique<PtrField>(type_i, std::move(original_field), val, name, neu_getter,T_POINTER, 0, 0);
        } else if constexpr (is_unique<T>::value) {
            std::function<std::any(const std::any&)> neu_getter = [getter](const std::any&val) {
                return std::any_cast<T>(getter(val)).get();
            };
            std::unique_ptr<Field> original_field = flatten_type_to_enum<T::element_type>(val, name, [neu_getter](const std::any& val) { return *std::any_cast<T*>(neu_getter(val));});
            return  std::make_unique<PtrField>(type_i, std::move(original_field), val, name, neu_getter,T_POINTER, 0, 0);
        }
    }
    else if constexpr (is_variant<T>::value) {
        ReflectionManager::reflection_record_create<T>();
        std::function<uint64_t(const std::any&)> f = [getter](const std::any& val) {
            return std::any_cast<T>(getter(val)).index();
        };
        return  std::make_unique<VariantField>(type_i, std::move(f), val, name, getter,T_VARIANT, 0, 0);
    }
    else if constexpr (std::is_function_v<T>) {
        return  std::make_unique<Field>(type_i, val, name, getter,  T_FUNCTION, 0, 0);
    }
    else if constexpr (std::is_pointer_v<T>) {
        std::unique_ptr<Field> original_field = flatten_type_to_enum<std::remove_pointer_t<T>>(val, name, [getter](const std::any& val) { return *std::any_cast<T*>(getter(val));});
        return  std::make_unique<PtrField>(type_i, std::move(original_field), val, name, getter,T_POINTER, 0, 0);
    }
    else if constexpr (std::is_class_v<T>) {
        ReflectionManager::reflection_record_create<T>();
        return  std::make_unique<Field>(type_i, val, name, getter,T_CLASS, 0, 0);
    }
    return  std::make_unique<Field>(type_i, val, name, getter,T_UNEXPECTED, 0, 0);
}

#endif //NARCISSUS_FLATTEN_TYPE_TO_ENUM_H