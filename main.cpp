// main.cpp
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

#include <iostream>

// #include "submodule/field-reflection/include/field_reflection.hpp"

#include <functional>
#include <any>
#include <memory>

// #define MAGIC_ENUM_AUTO_IS_FLAGS
#include <narcissus/reflection/Reflection.h>
#include <narcissus/reflection/ReflectionManager.h>
#include <narcissus/reflection/fields/EnumField.h>
#include <narcissus/reflection/fields/PtrField.h>

#include "narcissus/gsm/gsm.h"
// #include "submodule/magic_enum/include/magic_enum/magic_enum.hpp"

enum class testing_enum : int64_t {
    A_VAL = -20,
    B_VAL = -30,
    C_VAL = 0,
    D_VAL = 10
};

struct other_elements {
    double value;
    other_elements* self;
    std::shared_ptr<other_elements> othero;
};

struct testing {
    std::string element;
    int value;
    testing_enum values;
    testing* self;
    std::tuple<int, double, std::string> tup_tup;
    std::variant<float, std::string, bool> var_elem;
    std::list<testing> children;
    other_elements other;
    std::array<double, 10> bounded_array;
    // double bounded_array[1];
    // int things[];
};

#include <rfl/json.hpp>
// #include <rfl/yaml.hpp>
#include <rfl.hpp>

enum SerializationFormat {
    JSON_FORMAT,
    YAML_FORMAT
};

class Serialize {
    std::map<std::uintptr_t, uint64_t> serialization;
    std::vector<GSMObject> container;
    ReflectionManager manager;

public:

    void deserialize(SerializationFormat format, std::istream& _stream) {
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

    void serialize(SerializationFormat format, std::ostream& _stream) {
        switch (format) {
            case JSON_FORMAT:
                rfl::json::write(container, _stream, YYJSON_READ_JSON5);
                break;
            case YAML_FORMAT:
                // rfl::yaml::write(container, _stream);
                break;
        }
    }


    void extract_information(Reflection *rt,
                             const lightweight_any& obj,
                             const Field* field_ptr,
                             std::string field_name,
                             std::function<void(const std::string&,const std::variant<std::string,Phi>&)> f) {
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

    bool serialize_to_cache(/*std::uintptr_t address,*/ Reflection *rt, lightweight_any obj) {
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

    template <typename T>
    bool serialize_to_cache(const T *ptr, Reflection* rt) {
        lightweight_any obj{ptr};
        return serialize_to_cache(rt, obj);
    }

    template<typename T> bool serialize_to_cache(const T* ptr) {
        std::uintptr_t address = (uintptr_t)(void*)ptr;
        auto rt = ReflectionManager::reflection_from_type<T>();
        assert(rt != nullptr);
        return serialize_to_cache<T>(ptr, rt);
    }

    template<typename T> bool serialize_to_cache(const T& original_value) {
        const T* ptr = &original_value;
        return serialize_to_cache<T>(ptr);
    }
};

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    testing tv{"ciao mondo", 123, testing_enum::B_VAL};
    tv.bounded_array = {0,1,2,3,4,5,6,7,8,123.5};
    tv.tup_tup = {1, 123.987, "mondo"};
    tv.self = &tv;
    tv.var_elem = "capovaro_ririprovo?"; // https://www.youtube.com/watch?v=bsm1U2NWjhI&t=112s
    tv.other.value = 10.0;
    tv.other.self = &tv.other;
    tv.other.othero = std::make_shared<other_elements>();
    tv.other.othero->value = 20.0;
    tv.other.othero->self = tv.other.othero.get();
    tv.other.othero->othero= nullptr;
    {
        auto& ref = tv.children.emplace_back();
        ref.element = "child#1";
        ref.value = 1;
    }
    {
        auto& ref = tv.children.emplace_back();
        ref.element = "child#2";
        ref.value = 2;
    }

    Serialize serializer;
    serializer.serialize_to_cache(tv);
    {
        std::ofstream file{"/home/gyankos/CLionProjects/reflection/test.json"};
        serializer.serialize(JSON_FORMAT, file);
    }
        exit(2);

    // std::any any_test{&tv};
    // std::cout << std::get<std::string>(std::any_cast<testing*>(any_test)->var_elem) << std::endl;


    auto t = ReflectionManager::reflection_from_type<testing>();

    // std::shared_ptr<remove_arg<std::shared_ptr<std::string>>::type> g{nullptr};
    // auto w = g;
    // std::array<int, 10> bounded_array;
    // int* val = bounded_array.data();
    // Reflection t = Reflection::from_type<testing>();
    std::cout << t->getName() << std::endl;

    std::cout << *t->getField<std::string>("element", &tv) << std::endl;

    // std::cout << std::any_cast<std::string>(t->getField("element")->any_value(tv)) << std::endl;
    auto for_enum = (EnumField*)t->getField("values");
    std::cout << for_enum->toString(testing_enum::C_VAL) << std::endl;
    auto dis = t->getField<testing_enum>("values", &tv);
    {
        auto for_ptr = (PtrField*)t->getField("self");
        auto tv_ptr = t->getField<testing>("self", &tv);
        auto other_field = t->getField("other");
        std::cout << other_field->get_field_name() << std::endl;
        auto other_other = other_field->asReflection()->getField("othero");
        std::cout << other_other->get_field_name() << std::endl;
        auto resulto = other_other->value<other_elements>(&tv.other);
    }

    {
        auto ptr = (ArrayField*)t->getField("bounded_array");
        for (uint64_t idx = 0, N = ptr->size_if_bounded_array(); idx < N; idx++) {
            *ptr->at(idx)->value<double>(&tv) = idx*20.0+1.0;
        }
        for (uint64_t idx = 0, N = ptr->size_if_bounded_array(); idx < N; idx++) {
            std::cout << *ptr->at(idx)->value<double>(&tv) << std::endl;
        }
    }

    {
        auto ptr = (DynamicArrayField*)t->getField("children");
        for (uint64_t idx = 0, N = ptr->dynamic_size(&tv); idx < N; idx++) {
            auto field_ptr = ptr->at(idx);
            auto val = field_ptr->value<testing>(&tv);
            std::cout << field_ptr->get_field_name() << std::endl;
        }
    }

    auto tup = t->getField("tup_tup");

    auto tup_refl = tup->asReflection()->getField("tuple_field_0")->value<int>(&tv.tup_tup);
    auto tup_refl2 = tup->asReflection()->getField("tuple_field_1")->value<double>(&tv.tup_tup);
    auto tup_refl3 = tup->asReflection()->getField("tuple_field_2")->value<std::string>(&tv.tup_tup);
    // ((PtrField*)t->getField("self"))->any_value
    // auto t_other = ((PtrField*)t->getField("self"))->getOriginalField()->asReflection()->getField("element");

    auto var = t->getField("var_elem");
    std::cout << *var->value<std::string>(&tv) << std::endl;

    return 0;
    // TIP See CLion help at <a href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>. Also, you can try interactive lessons for CLion by selecting 'Help | Learn IDE Features' from the main menu.
}