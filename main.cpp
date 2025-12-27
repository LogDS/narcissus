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

#include "submodule/field-reflection/include/field_reflection.hpp"

#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>
#include <functional>
#include <any>
#include <memory>
#include <queue>
#include <stack>
#include <typeindex>





#define MAGIC_ENUM_AUTO_IS_FLAGS
#include "narcissus/reflection/Reflection.h"
#include "narcissus/reflection/ReflectionManager.h"
#include "narcissus/reflection/fields/EnumField.h"
#include "submodule/magic_enum/include/magic_enum/magic_enum.hpp"

enum class testing_enum  {
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
    std::vector<testing> children;
    other_elements other;
};

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    testing tv{"ciao mondo", 123, testing_enum::B_VAL};
    tv.self = &tv;
    tv.other.value = 10.0;
    tv.other.self = &tv.other;
    tv.other.othero = std::make_shared<other_elements>();
    tv.other.othero->value = 20.0;
    tv.other.othero->self = tv.other.othero.get();
    tv.other.othero->othero= nullptr;
    ReflectionManager manager;
    auto t = manager.reflection_from_type<testing>();

    std::shared_ptr<remove_arg<std::shared_ptr<std::string>>::type> g{nullptr};
    auto w = g;

    // Reflection t = Reflection::from_type<testing>();
    std::cout << t->getName() << std::endl;

    std::cout << t->getField<std::string>("element", tv) << std::endl;

    // std::cout << std::any_cast<std::string>(t->getField("element")->any_value(tv)) << std::endl;
    auto for_enum = (EnumField*)t->getField("values");
    std::cout << for_enum->toString(testing_enum::C_VAL) << std::endl;

    auto tv_ptr = t->getField<testing*>("self", tv);

    auto other_field = t->getField("other");
    std::cout << other_field->get_field_name() << std::endl;
    auto other_other = other_field->asReflection()->getField("othero");
    std::cout << other_other->get_field_name() << std::endl;
    auto resulto = other_other->value<other_elements*>(tv.other);
    // ((PtrField*)t->getField("self"))->any_value
    // auto t_other = ((PtrField*)t->getField("self"))->getOriginalField()->asReflection()->getField("element");



    return 0;
    // TIP See CLion help at <a href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>. Also, you can try interactive lessons for CLion by selecting 'Help | Learn IDE Features' from the main menu.
}