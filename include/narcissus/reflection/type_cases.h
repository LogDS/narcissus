// type_cases.h
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

#ifndef NARCISSUS_TYPE_CASES_H
#define NARCISSUS_TYPE_CASES_H

#include <cstdint>

enum type_cases : uint8_t {
    T_VOID = 1,
    T_NULLPTR = 2,
    T_SIGNED_INTEGRAL = 3,
    T_U_INTEGRAL = 4,
    T_SIGNED_FLOAT = 5,
    T_U_FLOAT = 6,
    T_STATIC_ARRAY = 7,
    T_OTHER_ARRAY = 8,
    T_ENUM = 9,
    T_UNION = 10,
    T_CLASS = 11,
    T_FUNCTION = 12,
    T_POINTER = 13,
    T_STRING = 14,
    T_TUPLE = 15,
    T_VARIANT = 16,
    T_UNEXPECTED = 0,
};

#include <narcissus/template_typing.h>

template <typename T> constexpr type_cases getTypeInformation() {
    if constexpr (std::is_same_v<T, std::string>) {
        return type_cases::T_STRING;
    } else if constexpr (std::is_void<T>::value) {
        return type_cases::T_VOID;
    }if constexpr (std::is_null_pointer<T>::value) {
        return type_cases::T_NULLPTR;
    } else if constexpr (std::is_integral<T>::value) {
        if constexpr (std::is_signed<T>::value) {
            return type_cases::T_SIGNED_INTEGRAL;
        } else {
            return type_cases::T_U_INTEGRAL;
        }
    } else if constexpr (std::is_floating_point<T>::value) {
        if constexpr (std::is_signed<T>::value) {
            return type_cases::T_SIGNED_FLOAT;
        } else {
            return type_cases::T_U_FLOAT;
        }
    } else if constexpr (is_std_array<T>::value || std::is_array<T>::value) {
        return type_cases::T_STATIC_ARRAY;
    } else if constexpr (is_vector<T>::value) {
        return type_cases::T_OTHER_ARRAY;
    } else if constexpr  (is_list<T>::value) {
        return type_cases::T_OTHER_ARRAY;
    }  else if constexpr (std::is_enum<T>::value) {
        return type_cases::T_ENUM;
    } else if constexpr (is_tuple<T>::value) {
        return type_cases::T_TUPLE;
    } else if constexpr (std::is_union<T>::value) {
        return type_cases::T_UNION;
    } else if constexpr (is_smart_pointer<T>::value || is_actual_pointer<T>::value) {
        return type_cases::T_POINTER;
    } else if constexpr (is_variant<T>::value) {
        return type_cases::T_VARIANT;
    } else if constexpr (std::is_function<T>::value) {
        return type_cases::T_FUNCTION;
    } else if constexpr (std::is_class<T>::value) {
        return type_cases::T_CLASS;
    } else {
        return type_cases::T_UNEXPECTED;
    }
}


#endif //NARCISSUS_TYPE_CASES_H