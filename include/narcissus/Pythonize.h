// Pythonize.h
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
// Created by gyankos on 29/12/25.
//

#ifndef NARCISSUS_PYTHONIZE_H
#define NARCISSUS_PYTHONIZE_H

#include <iostream>

// #include "submodule/field-reflection/include/field_reflection.hpp"

#include <functional>
#include <any>
#include <memory>

// #define MAGIC_ENUM_AUTO_IS_FLAGS
#include <narcissus/reflection/Reflection.h>
#include <narcissus/reflection/ReflectionManager.h>
#include <narcissus/reflection/fields/EnumField.h>
#include <narcissus/reflection/fields/PtrField.h>

struct Pythonize {
    Pythonize();
    Pythonize(const lightweight_any& object, Reflection* refl, Field* fld);

    template <typename T> Pythonize(T* object, Reflection* refl) : any{object}, refl{refl}, fld{fld} {
    }

    template <typename T> Pythonize(T* object) : any{object}, fld(nullptr) {
        refl = ReflectionManager::reflection_from_type<T>();
    }

    std::vector<std::string> keys() const;

    /**
     * Attempting to cast non-basic members to base C++ constructs
     *
     * @tparam K Type towards which attempt to perform the casting
     * @return If the cast is insuccessful, either nullptr or an exception is thrown
     */
    template<typename K> K* get() const {
        if (((refl) && refl->type() == T_ENUM) || ((!refl) && (fld) && fld->type() == T_ENUM)) {
            if constexpr (std::is_enum_v<K>) {
                return (K*)((EnumField*)fld)->toEnumPtr(any);
            } else if constexpr (std::is_same_v<K, std::string>) {
                return (K*)((EnumField*)fld)->toStringPtr(any);
            } else if constexpr (std::is_integral<K>::value) {
                return (K*)((EnumField*)fld)->toIndexPtr(any);
            } else {
                return nullptr;
            }
        }
        if (((refl) && refl->type() == T_SIGNED_FLOAT) || ((!refl) && (fld) && fld->type() == T_SIGNED_FLOAT)) {
            return any.get<K>();
        }
        if (((refl) && refl->type() == T_U_INTEGRAL) || ((!refl) && (fld) && fld->type() == T_U_INTEGRAL)) {
            return any.get<K>();
        }
        if (((refl) && refl->type() == T_SIGNED_INTEGRAL) || ((!refl) && (fld) && fld->type() == T_SIGNED_INTEGRAL)) {
            return any.get<K>();
        }
        if (((refl) && refl->type() == T_STRING) || ((!refl) && (fld) && fld->type() == T_STRING)) {
            return any.get<K>();
        }
        return any.get<K>();
    }

    /**
     *
     * @return returns true if the object shall be considered as a null pointer
     */
    bool is_null() const;

    /**
     *
     * @return If an object is a tuple or a vector, it returns its size
     */
    uint64_t size() const;

    /**
     * This returns the fields for Tuples or arrays/vectors
     * @param idx Index for accessing the desired element
     * @return a null Pythonize if there is no such element, and its resolution otherwise
     */
    Pythonize operator[](const uint64_t idx);

    /**
     * This structure allows you to crawl the object as it was a Python dictionary
     * @param key The field to access on the data structure
     * @return a null Pythonize if the field is not there, and its resolution otherwise
     */
    Pythonize operator[](const std::string& key);

private:
    lightweight_any any;
    Reflection* refl;
    Field* fld;
};

/*
*    Example:
*        testing tv{"ciao mondo", 123, testing_enum::B_VAL};
tv.bounded_array = {0,1,2,3,4,5,6,7,8,123.5};
tv.tup_tup = {1, 123.987, "mondo"};
tv.self = &tv;
tv.var_elem = "capovaro_ririprovo?"; // https://www.youtube.com/watch?v=bsm1U2NWjhI&t=112s
tv.other.value = 10.0;
tv.other.self = &tv.other;
tv.other.othero = std::make_shared<other_elements>();
tv.other.othero->value = 20.0;
tv.other.othero->self = tv.other.othero.get();
tv.other.othero->othero= nullptr;
{
auto& ref = tv.children.emplace_back();
ref.element = "child#1";
ref.value = 1;
}
{
auto& ref = tv.children.emplace_back();
ref.element = "child#2";
ref.value = 2;
}
*
*
*    Pythonize p(&tv);
auto test = p["value"];
std::cout << *test.get<int>() << std::endl;
auto mondo = p["element"];
std::cout << *mondo.get<std::string>() << std::endl;
auto values = p["values"];
std::cout << *values.get<std::string>() << std::endl;
auto capovar = p["var_elem"];
std::cout << *capovar.get<std::string>() << std::endl;
auto bounding = p["bounded_array"];
std::cout << bounding.size() << std::endl;
std::cout << *bounding[9].get<double>() << std::endl;
auto children = p["children"];
auto child_0 = children[0];
std::cout << *child_0["element"].get<std::string>() << std::endl;
*/

#endif //NARCISSUS_PYTHONIZE_H
