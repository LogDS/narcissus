// ReflectionManager.h
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

#ifndef NARCISSUS_REFLECTIONMANAGER_H
#define NARCISSUS_REFLECTIONMANAGER_H


// class Reflection;

#include <narcissus/reflection/Reflection.h>
#include <narcissus/reflection/ReflectionManagerData.h>
#include <unordered_map>
#include <field_reflection.hpp>
// #include <narcissus/reflection/field.h>

class ReflectionManager {
    static std::unordered_map<std::type_index, ReflectionManagerData> reflections_info;
    static std::unordered_map<std::string, std::type_index> name_to_index;
    // std::unordered_map<std::string, std::unique_ptr<Field>> fields;

public:
    template<typename _Tp> static Reflection* reflection_from_type() {
        return reflection_record_create<_Tp>()->reflection.get();
    }

    template<typename _Tp> static  Field* field_from_type() {
        return reflection_record_create<_Tp>()->as_field.get();
    }
    // template<typename _Tp> static ReflectionManagerData* reflection_record_create();
    static Reflection* reflection_from_type_index(const std::type_index& x);
    static Field* field_from_type_index(const std::type_index& x);
    static Reflection* reflection_from_name(const std::string& sv);

    template<typename _Tp> static Field* field_from_name(const std::string& sv) {
        auto it = name_to_index.find(std::string(sv));
        if (it == name_to_index.end())
            return nullptr;
        return reflections_info.find(it->second)->second.as_field.get();
    }
    template<typename _Tp> static ReflectionManagerData* reflection_record_create();
};

#include "flatten_type_to_enum.h"

template<typename _Tp> ReflectionManagerData* ReflectionManager::reflection_record_create() {
    auto info = std::type_index(typeid(_Tp));
    // auto val = std::string(field_reflection::type_name<_Tp>);
    auto it = reflections_info.find(info);
    if (it == reflections_info.end()) {
        it = reflections_info.emplace(info, info).first;
        it->second.name = std::string(field_reflection::type_name<_Tp>);
        assert(!name_to_index.contains(it->second.name));
        name_to_index.emplace(it->second.name, info);
        it->second.as_field = flatten_type_to_enum<_Tp>(-1, std::string(field_reflection::type_name<_Tp>), [](const lightweight_any& x) {return x;});
        it->second.reflection = Reflection::from_type<_Tp>(it->second.as_field.get());
    }
    return &it->second;
}

#endif //NARCISSUS_REFLECTIONMANAGER_H