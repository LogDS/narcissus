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
#include <any>
#include <functional>

// #include "ReflectionManager.h"
#ifdef GSM
#include <narcissus/gsm/gsm.h>
#endif
// #include <narcissus/reflection/fields/EnumField.h>

class Reflection {
    std::vector<std::unique_ptr<Field>> fields;
    std::unordered_map<std::string, uint64_t> field_map;
    Field* self;
    std::function<std::string(const std::any&)> string_stream;
    std::function<void*(const std::any&)> void_ptr;
    bool isEnumerated, streamable;

public:
    Reflection(Field* self,
           std::vector<std::unique_ptr<Field>>&& fields,
           bool is_streamable,
           std::function<std::string(const std::any&)>&& string_stream,
           bool isEnumerated,
           std::function<void*(const std::any&)>&& as_void_pointer);

    template<typename T> static std::unique_ptr<Reflection> from_type(Field* field_orig);

    template <typename K> K getField(const std::string& name, const std::any& val);

    const std::string_view getName();

    const uint64_t getFieldSize() {
        return isEnumerated ? 0 : fields.size();
    }

    const Field* getField(const std::string& name) {
        if (isEnumerated)
            return nullptr;
        auto it = field_map.find(name);
        if (it == field_map.end()) {
            return nullptr;
        }
        return fields[it->second].get();
    }

    type_cases type();

    uint64_t size_if_bounded_array() const;

#ifdef GSM
    std::string toString(const std::any& obj, bool c_default_repr) {
        switch (self->type()) {
            case T_VOID:
                return "void";
            case T_NULLPTR:
                return "null";
            case T_SIGNED_INTEGRAL:
                if ( self->size_type() == 8) {
                    return (std::to_string(std::any_cast<int8_t>(self->any_value(obj))));
                } else if ( self->size_type() == 16) {
                    return (std::to_string(std::any_cast<int16_t>(self->any_value(obj))));
                } else if ( self->size_type() == 32) {
                    return (std::to_string(std::any_cast<int32_t>(self->any_value(obj))));
                } else if ( self->size_type() == 64) {
                    return (std::to_string(std::any_cast<int64_t>(self->any_value(obj))));
                }
                break;
            case T_U_INTEGRAL:
                if ( self->size_type() == 8) {
                    return (std::to_string(std::any_cast<uint8_t>(self->any_value(obj))));
                } else if ( self->size_type() == 16) {
                    return (std::to_string(std::any_cast<uint16_t>(self->any_value(obj))));
                } else if ( self->size_type() == 32) {
                    return (std::to_string(std::any_cast<uint32_t>(self->any_value(obj))));
                } else if ( self->size_type() == 64) {
                    return (std::to_string(std::any_cast<uint64_t>(self->any_value(obj))));
                }
                break;
            case T_SIGNED_FLOAT:
                if ( self->size_type() == sizeof(float)) {
                    return (std::to_string(std::any_cast<float>(self->any_value(obj))));
                } else if ( self->size_type() == sizeof(double)) {
                    return (std::to_string(std::any_cast<double>(self->any_value(obj))));
                } else if ( self->size_type() == sizeof(long double)) {
                    return (std::to_string(std::any_cast<long double>(self->any_value(obj))));
                }
                break;
            case T_STATIC_ARRAY:
                break;
            case T_OTHER_ARRAY:
                break;
            case T_ENUM:
                break;
            case T_UNION:
                break;
            case T_CLASS:
                if (c_default_repr && streamable) {
                    return string_stream(obj);
                } else {
                    std::unordered_map<uint64_t, GSMObject> result_map;
                    fromObject(obj, result_map);
                    // TODO: database serialization of result_map
                    return "";
                }
            case T_POINTER:
                break;
            case T_STRING: {
                std::stringstream os;
                os << std::quoted(std::any_cast<std::string>(obj));
                return os.str();
            }
        }
        throw std::runtime_error("Unsigned floats were not currently supported");
    }

    void fromObject(const std::any& obj, std::unordered_map<uint64_t, GSMObject>& result_map) {
        if (!obj.has_value())
            return;
        auto ptr_id = (uint64_t)void_ptr(obj);
        if (result_map.contains(ptr_id))
            return;
        GSMObject& result = result_map[ptr_id];
        result.id.set(ptr_id);
        // auto& reflection =  from_type<T>();
        std::unordered_map<std::string, std::vector<std::string>> properties;
        result.ell.set({std::string(getName())});
        for (uint64_t i = 0; i < fields.size(); ++i) {
            auto field_ptr = fields.at(i).get();
            auto field_name = std::string(field_ptr->get_field_name());
            type_cases val = field_ptr->type();
            switch (val) {
                case T_VOID:
                    properties[field_name] = {"void"};
                    break;
                case T_NULLPTR:
                    properties[field_name] = {"null"};
                    break;
                case T_SIGNED_INTEGRAL:
                    if ( field_ptr->size_type() == 8) {
                        properties[field_name].emplace_back(std::to_string(std::any_cast<int8_t>(field_ptr->any_value(obj))));
                    } else if ( field_ptr->size_type() == 16) {
                        properties[field_name].emplace_back(std::to_string(std::any_cast<int16_t>(field_ptr->any_value(obj))));
                    } else if ( field_ptr->size_type() == 32) {
                        properties[field_name].emplace_back(std::to_string(std::any_cast<int32_t>(field_ptr->any_value(obj))));
                    } else if ( field_ptr->size_type() == 64) {
                        properties[field_name].emplace_back(std::to_string(std::any_cast<int64_t>(field_ptr->any_value(obj))));
                    }
                    break;
                case T_U_INTEGRAL:
                    if ( field_ptr->size_type() == 8) {
                        properties[field_name].emplace_back(std::to_string(std::any_cast<uint8_t>(field_ptr->any_value(obj))));
                    } else if ( field_ptr->size_type() == 16) {
                        properties[field_name].emplace_back(std::to_string(std::any_cast<uint16_t>(field_ptr->any_value(obj))));
                    } else if ( field_ptr->size_type() == 32) {
                        properties[field_name].emplace_back(std::to_string(std::any_cast<uint32_t>(field_ptr->any_value(obj))));
                    } else if ( field_ptr->size_type() == 64) {
                        properties[field_name].emplace_back(std::to_string(std::any_cast<uint64_t>(field_ptr->any_value(obj))));
                    }
                    break;
                case T_SIGNED_FLOAT:
                    if ( field_ptr->size_type() == sizeof(float)) {
                        properties[field_name] = {std::to_string(std::any_cast<float>(field_ptr->any_value(obj)))};
                    } else if ( field_ptr->size_type() == sizeof(double)) {
                        properties[field_name] = {std::to_string(std::any_cast<double>(field_ptr->any_value(obj)))};
                    }
                    break;
                case T_U_FLOAT:
                    throw std::runtime_error("Unsigned floats were not currently supported");
                    break;
                case T_STATIC_ARRAY:
                    break;
                case T_ENUM:
                    properties[field_name].emplace_back(((EnumField*)field_ptr)->toString(field_ptr->any_value(obj)));
                    break;
                case T_UNION:
                    throw std::runtime_error("Unsigned floats were not currently supported. Please consider using variants!");
                case T_CLASS: {

                }
                    break;
                case T_FUNCTION:
                    throw std::runtime_error("Functions were not currently supported");
                    break;
                case T_POINTER:
                    break;
                case T_STRING: {
                    std::stringstream os;
                    os << std::quoted(std::any_cast<std::string>(field_ptr->any_value(obj)));
                    properties[field_name].emplace_back(os.str());
                }
                    break;
                case T_UNEXPECTED:
                    throw std::runtime_error("Unexpected type!");
            }
        }

        // std::unordered_map<std::string, std::string> final_properties;
        // for (const auto& [k, v] : properties) {
        //     if (v.size() == 1) {
        //         final_properties[k] = v[0];
        //     } else if (v.size() > 1) {
        //         const char* const delim = ", ";
        //         std::ostringstream imploded;
        //         imploded << "[";
        //         std::copy(v.begin(), v.end(),
        //                    std::ostream_iterator<std::string>(imploded, delim));
        //         imploded << "]";
        //         final_properties[k] = imploded.str();
        //     }
        // }
        result.properties.set(properties);
        if (streamable) {
            std::stringstream ss;
            ss << string_stream(obj);
            result.xi.set({ss.str()});
        }
    }
#endif
};

#include "ReflectionManager.h"



template<typename T, int x, int to>
struct static_for
{
    std::vector<std::unique_ptr<Field>> tuple_enum() {
        auto result = static_for<T, x+1,to>().tuple_enum();
        std::function<std::any(const std::any&)> f = [](const std::any& val) {
            return std::get<x>(std::any_cast<T>(val));
        };
        result.emplace_back(flatten_type_to_enum<std::tuple_element<x,T>>(x, "tuple_field_"+std::to_string(x), (f)));
        return result;
    }
    std::vector<std::unique_ptr<Field>> field_enum() {
        auto result = static_for<T, x+1,to>().field_enum();
        std::function<std::any(const std::any&)> f = [](const std::any& val) {
            return field_reflection::get_field<x>(std::any_cast<T>(val));
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

    std::vector<std::unique_ptr<Field>> field_enum() {
        return {};
    }
};

#include <narcissus/reflection/fields/Field.h>

template<typename T>
std::unique_ptr<Reflection> Reflection::from_type(Field *field_orig) {
    std::function<std::string(const std::any&)> f = [](const std::any&){ return std::string(""); };
    std::function<void*(const std::any&)> as_void_pointer = [](const std::any& val) {
        const auto& x = std::any_cast<const T&>(val);
        return (void*)&x;
    };
    bool streamable = false;
    if constexpr (is_streamable<T>::value) {
        streamable = true;
        f = [](const std::any& x) {
            std::stringstream ss;
            ss << std::any_cast<T>(&x);
            return ss.str();
        };
    }
    // else {
    //     f = [](const std::any& x) {
    //         return std::to_string(std::any_cast<T>(x));
    //     };
    // }
    std::vector<std::unique_ptr<Field>> fields;
    if constexpr (is_tuple<T>::value) {
        fields = static_for<T, 0,  std::tuple_size_v<T>>{}.tuple_enum();
        std::reverse(fields.begin(), fields.end());
    } else if constexpr (std::is_class_v<T> && (!is_smart_pointer<T>::value) && (!std::is_same_v<T, std::string>) && (!is_vector<T>::value) && (!is_list<T>::value) && (!is_deque<T>::value) && (!is_stack<T>::value) && (!is_queue<T>::value) && (!is_map<T>::value) && (!is_unordered_map<T>::value)) {
        fields  = static_for<T, 0, field_reflection::field_count<T>>{}.field_enum();
        std::reverse(fields.begin(), fields.end());
    }
    auto isEnumerated = std::is_enum_v<T>;
    return std::make_unique<Reflection>(field_orig, std::move(fields), streamable, std::move(f), isEnumerated, std::move(as_void_pointer));
}

template <typename K> K Reflection::getField(const std::string& name, const std::any& val) {
    auto f = getField(name);
    if (f == nullptr)
        throw std::runtime_error("Missing field: "+name);
    return f->value<K>(val);
}

#endif //NARCISSUS_REFLECTION_H
