// PtrField.cpp
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

#include <narcissus/reflection/fields/PtrField.h>

PtrField::PtrField(const std::type_index &val_t, std::unique_ptr<Field> &&actual_val, uint64_t val,
    const std::string& name, std::function<std::any(const std::any &)> getter, type_cases cases_,
    uint64_t bounded_array_size, uint64_t size_):     Field(val_t, val, name, getter, cases_, bounded_array_size, size_), actual_val(std::move(actual_val)) {}
