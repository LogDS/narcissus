// Field.cpp
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
#include <narcissus/reflection/fields/Field.h>
#include <narcissus/reflection/ReflectionManager.h>

std::shared_ptr<Field> Field::redoMapping(std::function<lightweight_any(const lightweight_any &)> neu_getobj) const {
    return std::make_shared<Field>(val_t, idx, name, neu_getobj, case_, bounded_array_size, size_);
}

std::shared_ptr<Field> Field::copyAsShared() const {
    return std::make_shared<Field>(val_t, idx, name, getobj, case_, bounded_array_size, size_);
}

Class* Field::getClass() const {
    return ReflectionManager::reflection_from_type_index(val_t);
}

uint64_t Field::index() const {
    return idx;
}

lightweight_any Field::any_value(const lightweight_any &x) const {
    return getobj(x);
}

const std::string Field::get_field_name() const {
    return name;
}

type_cases Field::type() const {
    return case_;
}

uint64_t Field::size_type() const {
    return size_;
}

uint64_t Field::size_if_bounded_array() const {
    return bounded_array_size;
}
