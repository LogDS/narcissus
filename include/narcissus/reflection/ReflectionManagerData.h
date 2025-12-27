// ReflectionManagerData.h
// This file is part of reflection
//
// Copyright (C)  2025 - gyankos
//
// reflection is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  reflection is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with reflection. If not, see <http://www.gnu.org/licenses/>.

//
// Created by gyankos on 27/12/25.
//

#ifndef REFLECTION_REFLECTIONMANAGERDATA_H
#define REFLECTION_REFLECTIONMANAGERDATA_H

#include <memory>
#include <typeindex>

// #include <narcissus/reflection/Reflection.h>

class Reflection;
class Field;

struct ReflectionManagerData {
    std::string                 name;
    std::type_index              info;
    std::unique_ptr<Reflection> reflection ;
    std::unique_ptr<Field>      as_field ;

    ReflectionManagerData(const std::type_index& x); // : info{x} {}
};

#endif //REFLECTION_REFLECTIONMANAGERDATA_H