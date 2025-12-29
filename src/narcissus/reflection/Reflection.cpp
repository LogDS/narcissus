// Reflection.cpp
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

#include <narcissus/reflection/Reflection.h>
#include <narcissus/reflection/fields/Field.h>

Reflection::Reflection(Field *self, std::vector<std::unique_ptr<Field>> &&fields, bool is_streamable,
    std::function<std::string(const lightweight_any &)> &&string_stream, bool isEnumerated,
    std::function<void *(const lightweight_any &)> &&as_void_pointer): void_ptr{std::move(as_void_pointer)}, streamable{is_streamable},  self{self}, fields{std::move(fields)}, isEnumerated{isEnumerated}, string_stream{std::move(string_stream)} {
    for (uint64_t i = 0; i < this->fields.size(); ++i) {
        const auto name = std::string(this->fields[i]->get_field_name());
        field_map[name] = i;
    }
}

std::vector<std::string> Reflection::keys() const {
    std::vector<std::string> result;
    result.reserve(fields.size());
    for (uint64_t i = 0, N = fields.size(); i < N; i++) {
        result.emplace_back(fields[i]->get_field_name());
    }
    return result;
}

const std::string Reflection::getName() {
    return self->get_field_name();
}

type_cases Reflection::type() {
    return self->type();
}

uint64_t Reflection::size_if_bounded_array() const {
    return isEnumerated ? 0 : self->size_if_bounded_array();
}
