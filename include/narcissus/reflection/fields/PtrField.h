// PtrField.h
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

#ifndef NARCISSUS_PTRFIELD_H
#define NARCISSUS_PTRFIELD_H

#include <narcissus/reflection/fields/Field.h>

class PtrField : public Field {
    // uint64_t idx, size_;
    // std::string_view name;
    // std::function<std::any(const std::any &)> getobj;
    // type_cases case_;
    // uint64_t bounded_array_size;
    std::unique_ptr<Field> actual_val;

public:
    PtrField(const std::type_index &val_t,
             std::unique_ptr<Field> &&actual_val,
             uint64_t val,
             std::string_view name,
             std::function<std::any(const std::any &)> getter,
             type_cases cases_ = T_UNEXPECTED,
             uint64_t bounded_array_size = 0,
             uint64_t size_ = 0);

    /**
     *
     * @return Provides the reflection for the underlying type
     */
    Reflection *asReflection() const override {
        return getOriginalField()->asReflection();
    }

    /**
     * This method is useful to continue navigating the data structure
     * @return Returns the field type associated with the underlying type
     */
    const Field *getOriginalField() const {
        return actual_val.get();
    }
};

#endif //NARCISSUS_PTRFIELD_H
