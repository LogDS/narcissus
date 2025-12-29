// Reflection.h
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

#ifndef NARCISSUS_REFLECTION_H
#define NARCISSUS_REFLECTION_H
#include <memory>
#include <vector>
#include <sstream>

#include <field_reflection.hpp>
#include <narcissus/template_typing.h>

#include <narcissus/reflection/type_cases.h>
// #include <narcissus/reflection/fields/Field.h>

class Field;
#include <narcissus/lightweight_any.h>
#include <functional>

// #include "ReflectionManager.h"
#ifdef GSM
#include <narcissus/gsm/gsm.h>
#endif
// #include <narcissus/reflection/fields/EnumField.h>

class Class {
    std::vector<std::unique_ptr<Field>> fields;
    std::unordered_map<std::string, uint64_t> field_map;
    Field* self;
    std::function<std::string(const lightweight_any&)> string_stream;
    std::function<void*(const lightweight_any&)> void_ptr;
    bool isEnumerated, streamable;

public:
    Class(Field* self,
           std::vector<std::unique_ptr<Field>>&& fields,
           bool is_streamable,
           std::function<std::string(const lightweight_any&)>&& string_stream,
           bool isEnumerated,
           std::function<void*(const lightweight_any&)>&& as_void_pointer);

    template<typename T> static std::unique_ptr<Class> from_type(Field* field_orig);

    /**
     * Provides access to the data associated with the field name and belonging to an object val
     * @tparam K            Type to be casted
     * @param name          Name of the field to be accessed
     * @param val           Type-safe void* pointer passing à la std::any for the object belonging to the current class
     * @return              If the field with the given name is present and its value holds type of K and val is an object of the current class,
     *                      then it returns the pointer to the value stored in the original data. Otherwisem nullptr is returned
     */
    template <typename K> K* getField(const std::string& name, const lightweight_any& val);

    /**
     *
     * @return the accessible fields to the class
     */
    std::vector<std::string> keys() const;

    /**
     *
     * @return the class name
     */
    const std::string getName();

    /**
     *
     * @param name Name to be checked
     * @return Whether the class contains a field with a specific name
     */
    bool hasKey(const std::string& name) const;

    /**
     *
     * @return the number of the accessible fields
     */
    const uint64_t getFieldSize() const;

    /**
     *
     * @param idx the index of the field
     * @return Returns the class of the field if within the range, and nullptr otherwise
     */
    const Field* getField(uint64_t idx) const;

    /**
     *
     * @param name the name of the field to be accessed
     * @return Returns the class of the field if within the range, and nullptr otherwise
     */
    const Field* getField(const std::string& name) const;

    /**
     *
     * @return enum-based representation of non-parametric types
     */
    type_cases type();

    /**
     *
     * @return the size of the array if this is a static array (std::array<T, N>)
     */
    uint64_t size_if_bounded_array() const;
};

#include "ReflectionManager.h"



template<typename T, int x, int to>
struct static_for
{
    std::vector<std::unique_ptr<Field>> tuple_enum() {
        auto result = static_for<T, x+1,to>().tuple_enum();
        std::function<lightweight_any(const lightweight_any&)> f = [](const lightweight_any& val) {
            auto& ref = *val.get<T>();
            auto& result = std::get<x>(ref);
            return lightweight_any{&result};
            // return &std::get<x>(*val.get<T>());
        };
        result.emplace_back(flatten_type_to_enum<typename std::tuple_element<x,T>::type>(x, "tuple_field_"+std::to_string(x), (f)));
        return result;
    }
    std::vector<std::unique_ptr<Field>> variant_enum() {
        auto result = static_for<T, x+1,to>().variant_enum();
        std::function<lightweight_any(const lightweight_any&)> f = [](const lightweight_any& val) {
            auto& ref = *val.get<T>();
            auto& result = std::get<x>(ref);
            return lightweight_any{&result};
            // return &std::get<x>(*val.get<T>());
        };
        result.emplace_back(flatten_type_to_enum<std::variant_alternative_t<x,T>>(x, "variant_tag_"+std::to_string(x), (f)));
        return result;
    }
    std::vector<std::unique_ptr<Field>> field_enum() {
        auto result = static_for<T, x+1,to>().field_enum();
        std::function<lightweight_any(const lightweight_any&)> f = [](const lightweight_any& val) {
            auto& ref = *val.get<T>();
            auto& result = field_reflection::get_field<x>(ref);
            if constexpr (is_actual_pointer<decltype(result)>::value) {
                return lightweight_any{result};
            } else {
                auto* ptr = &result;
                return lightweight_any{ptr};
            }
        };
        result.emplace_back(flatten_type_to_enum<field_reflection::field_type<T, x>>(x, std::string(field_reflection::field_name<T, x>), (f)));
        return result;
    }
};

template<typename T, int to>
struct static_for<T, to,to>
{
    std::vector<std::unique_ptr<Field>> tuple_enum() {
        return {};
    }
    std::vector<std::unique_ptr<Field>> variant_enum() {
        return {};
    }

    std::vector<std::unique_ptr<Field>> field_enum() {
        return {};
    }
};

#include <narcissus/reflection/fields/Field.h>

template<typename T>
std::unique_ptr<Class> Class::from_type(Field *field_orig) {
    std::function<std::string(const lightweight_any&)> f = [](const lightweight_any&){ return std::string(""); };
    std::function<void*(const lightweight_any&)> as_void_pointer = [](const lightweight_any& val) {
        // const auto& x = std::any_cast<const T&>(val);
        return (void*)val.raw();
    };
    bool streamable = false;
    if constexpr (is_streamable2<T>::value) {
        streamable = true;
        f = [](const lightweight_any& x) {
            std::stringstream ss;
            ss << x.get<T>();
            return ss.str();
        };
    }
    std::vector<std::unique_ptr<Field>> fields;
    if constexpr (is_tuple<T>::value) {
        fields = static_for<T, 0,  std::tuple_size_v<T>>{}.tuple_enum();
        std::reverse(fields.begin(), fields.end());
    } else if constexpr (is_variant<T>::value) {
        fields = static_for<T, 0,  std::variant_size_v<T>>{}.variant_enum();
        std::reverse(fields.begin(), fields.end());
    } else if constexpr (std::is_class_v<T> && (!is_smart_pointer<T>::value) && (!std::is_same_v<T, std::string>) && (!is_vector<T>::value) && (!is_list<T>::value) && (!is_deque<T>::value) && (!is_stack<T>::value) && (!is_queue<T>::value) && (!is_map<T>::value) && (!is_unordered_map<T>::value)) {
        fields  = static_for<T, 0, field_reflection::field_count<T>>{}.field_enum();
        std::reverse(fields.begin(), fields.end());
    }
    auto isEnumerated = std::is_enum_v<T>;
    return std::make_unique<Class>(field_orig, std::move(fields), streamable, std::move(f), isEnumerated, std::move(as_void_pointer));
}

template <typename K> K* Class::getField(const std::string& name, const lightweight_any& val) {
    if (val.is_null())
        return nullptr;
    auto f = getField(name);
    if (f == nullptr)
        throw std::runtime_error("Missing field: "+name);
    return f->value<K>(val);
}

#endif //NARCISSUS_REFLECTION_H
