// serialization_example.cpp
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

#include <iostream>

// #include "submodule/field-reflection/include/field_reflection.hpp"

#include <functional>
#include <any>
#include <memory>

// #define MAGIC_ENUM_AUTO_IS_FLAGS
#include <narcissus/ORM.h>

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

int main(void) {
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

    ORM serializer;
    serializer.serialize_to_cache(tv);
    {
        std::ofstream file{"test.json"};
        serializer.serialize(JSON_FORMAT, file);
    }

    return EXIT_SUCCESS;
}