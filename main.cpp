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

    // std::any any_test{&tv};
    // std::cout << std::get<std::string>(std::any_cast<testing*>(any_test)->var_elem) << std::endl;

    ReflectionManager manager;
    auto t = manager.reflection_from_type<testing>();

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