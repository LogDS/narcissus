// Field.h
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

#ifndef NARCISSUS_FIELD_H
#define NARCISSUS_FIELD_H

#include <narcissus/lightweight_any.h>
#include <cinttypes>
#include <functional>
#include <string_view>
#include <typeindex>
#include <memory>

#include <narcissus/reflection/type_cases.h>



class Class;

class   Field {
    uint64_t idx, size_;
    std::string name;
    std::function<lightweight_any(const lightweight_any&)> getobj;
    type_cases case_;
    uint64_t bounded_array_size;
    std::type_index val_t;

public:
    Field(const std::type_index& val_t, uint64_t val, const std::string& name, std::function<lightweight_any(const lightweight_any&)> getter, type_cases cases_ = T_UNEXPECTED, uint64_t bounded_array_size = 0, uint64_t size_ = 0) : val_t{val_t},
    idx{val}, size_(size_), name{name}, getobj(getter), case_(cases_), bounded_array_size(bounded_array_size) {}

    virtual ~Field() = default;

    std::shared_ptr<Field> redoMapping(std::function<lightweight_any(const lightweight_any&)> neu_getobj) const;
    std::shared_ptr<Field> copyAsShared() const;

    /**
     *
     * @return the class-type information associated with the object
     */
    virtual Class* getClass() const;

    /**
     *
     * @return field number within a class of interest
     */
    uint64_t index() const;

    /**
     *
     * @param x   Type-safe pointer à la std::any of the class containing the field of interest
     * @return    Type-safe pointer to the object pointed by the field
     */
    virtual lightweight_any any_value(const lightweight_any&x) const;

    /**
     *
     * @tparam K  Expected C++ type associated with the object
     * @param x   Type-safe pointer à la std::any of the class containing the field of interest
     * @return    If the field associated is of the desired type
     */
    template <typename K> K* value(const lightweight_any&x) const {
        return any_value(x).get<K>();
    }

    /**
     *
     * @return Name of the field
     */
    const std::string get_field_name() const;

    /**
     *
     * @return Basic type associated with the field
     */
    type_cases type() const;

    /**
     *
     * @return If this points to a native C++ type, it returns the size of the object. This is important to disambiguate
     * the original type from the sign, real/integer, which is not sufficient to provide full type reconstruction
     */
    uint64_t size_type() const;

    /**
     *
     * @return If this points to a std::array<T,N>, this returns N
     */
    uint64_t size_if_bounded_array() const;
};


#endif //NARCISSUS_FIELD_H