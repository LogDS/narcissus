// ReflectionManager.cpp
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

#include <cassert>
#include <narcissus/reflection/ReflectionManager.h>
// #include <field_reflection.hpp>
// #include <any>
// #include <narcissus/reflection/Reflection.h>

std::unordered_map<std::type_index, ReflectionManagerData> ReflectionManager::reflections_info{};
std::unordered_map<std::string, std::type_index> ReflectionManager::name_to_index{};

Class * ReflectionManager::reflection_from_type_index(const std::type_index &x) {
    auto it = reflections_info.find(x);
    if (it == reflections_info.end())
        return nullptr;
    return it->second.reflection.get();
}

Field * ReflectionManager::field_from_type_index(const std::type_index &x) {
    auto it = reflections_info.find(x);
    if (it == reflections_info.end())
        return nullptr;
    return it->second.as_field.get();
}

Class * ReflectionManager::reflection_from_name(const std::string &sv) {
    auto it = name_to_index.find(std::string(sv));
    if (it == name_to_index.end())
        return nullptr;
    return reflections_info.find(it->second)->second.reflection.get();
}

