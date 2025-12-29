// ORM.cpp
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
// Created by gyankos on 29/12/25.
//

#include <narcissus/ORM.h>

void ORM::deserialize(SerializationFormat format, std::istream &_stream) {
    rfl::Result<std::vector<GSMObject>> result{rfl::Unexpected<rfl::Error>(rfl::Error{"No data was read!"})};
    switch (format) {
        case JSON_FORMAT: {
            result = rfl::json::read<std::vector<GSMObject>>(_stream, YYJSON_READ_JSON5);
        }
        break;
        case YAML_FORMAT: {
            // result = rfl::yaml::read<std::vector<GSMObject>>(_stream);
        }
        break;
    }
    if (result.has_value()) {
        auto& current = result.value();
        for (uint64_t i = 0, N = current.size(); i<N; i++) {
            serialization[(std::uintptr_t)&current[i]] = i;
        }
    }
}

void ORM::extract_information(Reflection *rt, const lightweight_any &obj, const Field *field_ptr,
    std::string field_name, std::function<void(const std::string &, const std::variant<std::string, Phi> &)> f) {
    // std::unordered_map<std::string, std::vector<std::string>> properties;
    if (field_name.empty()) {
        field_name = std::string(field_ptr->get_field_name());
    }
    type_cases val = field_ptr->type();
    switch (val) {
        case T_VOID:
            f(field_name,"void");
            return;
            break;

        case T_NULLPTR:
            f(field_name,"null");
            return;
            break;

        case T_SIGNED_INTEGRAL: {
            if (field_ptr->size_type() == sizeof(char)) {
                f(field_name,std::to_string(*(char*)(field_ptr->any_value(obj).raw())));
                return;
            } else if (field_ptr->size_type() == sizeof(int)) {
                f(field_name, std::to_string(*(int*)(field_ptr->any_value(obj).raw())));
                return;
            } else if ( field_ptr->size_type() == 8) {
                f (field_name,std::to_string(*(int8_t*)(field_ptr->any_value(obj).raw())));
                return;
            } else if ( field_ptr->size_type() == 16) {
                f(field_name,  std::to_string(*(int16_t*)(field_ptr->any_value(obj).raw())));
                return;
            } else if ( field_ptr->size_type() == 32) {
                f(field_name,  std::to_string(*(int32_t*)(field_ptr->any_value(obj).raw())));
                return;
            } else if ( field_ptr->size_type() == 64) {
                f(field_name, std::to_string(*(int64_t*)(field_ptr->any_value(obj).raw())));
                return;
            }
            break;
        }

        case T_U_INTEGRAL: {
            if (field_ptr->size_type() == sizeof(unsigned char)) {
                f(field_name,  std::to_string(*(unsigned char*)(field_ptr->any_value(obj).raw())));
                return;
            } else if (field_ptr->size_type() == sizeof(int)) {
                f(field_name,  std::to_string(*(unsigned int*)(field_ptr->any_value(obj).raw())));
                return;
            } else if ( field_ptr->size_type() == 8) {
                f (field_name, std::to_string(*(uint8_t*)(field_ptr->any_value(obj).raw())));
                return;
            } else if ( field_ptr->size_type() == 16) {
                f(field_name,  std::to_string(*(uint16_t*)(field_ptr->any_value(obj).raw())));
                return;
            } else if ( field_ptr->size_type() == 32) {
                f(field_name,  std::to_string(*(uint32_t*)(field_ptr->any_value(obj).raw())));
                return;
            } else if ( field_ptr->size_type() == 64) {
                f(field_name,  std::to_string(*(uint64_t*)(field_ptr->any_value(obj).raw())));
                return;
            }
            break;
        }

        case T_SIGNED_FLOAT: {
            if ( field_ptr->size_type() == sizeof(float)) {
                f(field_name,  std::to_string(*(float*)(field_ptr->any_value(obj).raw())));
            } else if ( field_ptr->size_type() == sizeof(double)) {
                f(field_name,  std::to_string(*(double*)(field_ptr->any_value(obj).raw())));
            }
            break;
        }

        case T_ENUM:
            f(field_name, ((EnumField*)field_ptr)->toString(field_ptr->any_value(obj)));
            return;
            break;

        case T_TUPLE: {
            auto obj3 = field_ptr->any_value(obj);
            auto rt2 = field_ptr->asReflection();
            auto N = rt2->getFieldSize();
            for (uint64_t idx = 0; idx < N; idx++) {
                auto tup_refl = rt2->getField("tuple_field_"+std::to_string(idx));
                // auto obj2 = tup_refl->any_value(obj3);
                extract_information(rt2, obj3, tup_refl, field_name+"_"+std::to_string(idx), f);
            }
            return;
            break;
        }
        case T_CLASS: {
            auto rt2 = field_ptr->asReflection();
            auto obj2 = field_ptr->any_value(obj);
            if (obj2.raw() != nullptr) {
                serialize_to_cache(rt2, obj2);
                Phi child;
                child.confidence = 1.0;
                child.containment = field_name;
                child.content = (uintptr_t)obj.raw();
                f(field_name, child);
            }
            return;
            break;
        }

        case T_POINTER: {
            auto for_ptr = (PtrField*)field_ptr;
            extract_information(for_ptr->getOriginalField()->asReflection(), obj, for_ptr->getOriginalField(), "", f);
            return;
            break;
        }

        case T_STRING: {
            // field_ptr->value<std::string>(obj));
            std::stringstream os;
            os << std::quoted(*(std::string*)field_ptr->any_value(obj).raw());
            f(field_name, os.str());
            return;
            break;
        }

        case T_STATIC_ARRAY: {
            auto ptr = (ArrayField*)field_ptr;
            for (uint64_t idx = 0, N = ptr->size_if_bounded_array(); idx < N; idx++) {
                auto i_field = ptr->at(idx);
                extract_information(i_field->asReflection(), obj, i_field.get(), field_name+"_"+std::to_string(idx), f);
                // std::cout << *ptr->at(idx)->value<double>(&tv) << std::endl;
            }
            return;
            break;
        }

        case T_OTHER_ARRAY:{
            auto ptr = (DynamicArrayField*)field_ptr;
            auto N = ptr->dynamic_size(obj);
            for (uint64_t idx = 0; idx < N; idx++) {
                auto i_field = ptr->at(idx);
                extract_information(i_field->asReflection(), obj, i_field.get(), field_name+"_"+std::to_string(idx), f);
                // std::cout << *ptr->at(idx)->value<double>(&tv) << std::endl;
            }
            return;
            break;
        }

        case T_VARIANT: {
            // auto var = t->getField("var_elem");
            auto for_ptr = (VariantField*)field_ptr;
            auto tmp3 = for_ptr->any_value(obj);
            auto idx = for_ptr->get_idx(obj);
            auto refl2 = for_ptr->asReflection();
            auto var = refl2->getField("variant_tag_"+std::to_string(idx))->redoMapping([](const auto& x) {return x;});
            // auto obj2 = var->any_value(obj);<
            extract_information(var->asReflection(),
                                tmp3,
                                var.get(),
                                field_name+"_"+std::to_string(idx),
                                f);
            break;
        }

        case T_U_FLOAT:
            throw std::runtime_error("Unsigned floats were not currently supported");
        case T_UNION:
            throw std::runtime_error("Unsigned floats were not currently supported. Please consider using variants!");
        case T_FUNCTION:
            throw std::runtime_error("Functions were not currently supported");
        case T_UNEXPECTED:
            throw std::runtime_error("Unexpected type!");
    }
}

bool ORM::serialize_to_cache(Reflection *rt, lightweight_any obj) {
    std::uintptr_t address = (uintptr_t)obj.raw();
    if (serialization.contains(address))
        return false;
    serialization[address] = container.size();
    GSMObject result;
    result.id = address;
    std::vector<std::string> ell, xi;
    ell.emplace_back(rt->getName());
    std::unordered_map<std::string, std::vector<std::string>> properties;

    std::vector<Phi> containments;
    for (uint64_t i = 0; i < rt->getFieldSize(); ++i) {
        auto* field_ptr = (Field*)rt->getField(i);
        std::function<void(const std::string&, const std::variant<std::string,Phi>&)> f = [&properties, &containments](const std::string& val, const std::variant<std::string,Phi>& tmp) {
            if (std::holds_alternative<std::string>(tmp)) {
                properties[val].emplace_back(std::get<std::string>(tmp));
            } else {
                assert(std::get<Phi>(tmp).containment.get() ==val);
                containments.emplace_back(std::move(std::get<Phi>(tmp)));
            }
        };
        extract_information(rt, obj, field_ptr, field_ptr->get_field_name(), f);

    }
    result.phi.set(containments);
    result.ell.set(ell);
    result.xi.set(xi);
    result.properties.set(properties);
    result.scores = {1.0};
    container.emplace_back(result);
    return true;
}
