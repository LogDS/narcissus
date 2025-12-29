// test.cpp
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
// Created by gyankos on 28/12/25.
//

#include <catch2/catch_test_macros.hpp>

#include <iostream>

// #include "submodule/field-reflection/include/field_reflection.hpp"

#include <functional>
#include <any>
#include <memory>

// #define MAGIC_ENUM_AUTO_IS_FLAGS
#include <narcissus/reflection/Class.h>
#include <narcissus/reflection/ReflectionManager.h>
#include <narcissus/reflection/fields/EnumField.h>

#include "catch2/matchers/catch_matchers.hpp"
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



TEST_CASE( "Factorials are computed", "[factorial]" ) {
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
    ReflectionManager manager;
    auto t = manager.reflection_from_type<testing>();

    SECTION( "basic testing" ) {
        CHECK( t->getName() == "testing");
    }

    SECTION("testing string") {
        CHECK( *t->getField<std::string>("element", &tv) == "ciao mondo");
    }

    SECTION("testing enum") {
        auto for_enum = (EnumField*)t->getField("values");
        CHECK(for_enum != nullptr);
        CHECK(for_enum->toString(testing_enum::C_VAL) == "C_VAL"); // for serialization
        CHECK(* t->getField<testing_enum>("values", &tv) == testing_enum::B_VAL);
    }

    SECTION("testing pointer #1") {
        auto tv_ptr = t->getField<testing>("self", &tv);
        CHECK(tv_ptr == tv.self);
    }

    SECTION("testing class struct #1") {
        auto other_field = t->getField("other");
        CHECK(other_field->get_field_name() == "other");
        auto other_other = other_field->getClass()->getField("othero");
    }

    SECTION("testing pointer #2") {
        auto other_field = t->getField("other");
        auto other = t->getField<other_elements>("other", &tv);
        CHECK(other != nullptr);
        CHECK(other == &tv.other);
        auto ptr = other_field->getClass();
        CHECK(ptr != nullptr);
        CHECK(ptr->getName() == "other_elements");
        auto other_other = other_field->getClass()->getField("othero");
        CHECK(other_other->get_field_name() == "othero");
        CHECK(other_other != nullptr);
        auto resulto = other_other->value<other_elements>(&tv.other);
        CHECK(resulto != nullptr);
        CHECK(resulto == tv.other.othero.get());
    }

    SECTION("Vector + Class testing ")     {
        auto ptr = (DynamicArrayField*)t->getField("children");
        CHECK(ptr != nullptr);
        CHECK(ptr->dynamic_size(&tv) == tv.children.size());
        auto it = tv.children.begin();
        for (uint64_t idx = 0, N = ptr->dynamic_size(&tv); idx < N; idx++) {
            auto field_ptr = ptr->at(idx);
            auto val = field_ptr->value<testing>(&tv);
            CHECK(val == (testing*)&*it);
            CHECK(field_ptr->get_field_name() == "children["+std::to_string(idx)+"]");
            it++;
        }
    }

    SECTION("tuple") {
        auto tup = t->getField("tup_tup");
        CHECK(tup != nullptr);
        auto refl = tup->getClass();
        CHECK(refl->getFieldSize() == 3);
        CHECK(tup->get_field_name() == "tup_tup");
        auto tup_refl = tup->getClass()->getField("tuple_field_0")->value<int>(&tv.tup_tup);
        CHECK(tup_refl == &std::get<0>(tv.tup_tup));
        auto tup_ptr = tup->value<std::tuple<int, double, std::string>>(&tv);
        CHECK(tup_ptr == &tv.tup_tup);
        auto tup_refl2 = tup->getClass()->getField("tuple_field_1")->value<double>(&tv.tup_tup);
        CHECK(tup_refl2 == &std::get<1>(tv.tup_tup));
        auto tup_refl3 = tup->getClass()->getField("tuple_field_2")->value<std::string>(&tv.tup_tup);
        CHECK(tup_refl3 == &std::get<2>(tv.tup_tup));
        auto other_ptr = t->getField<std::tuple<int, double, std::string>>("tup_tup", &tv);
        CHECK(other_ptr == &tv.tup_tup);
    }

    SECTION("variant") {
        auto var = t->getField("var_elem");
        CHECK(var != nullptr);
        CHECK(*var->value<std::string>(&tv) == "capovaro_ririprovo?");
    }

    SECTION("array")     {
        auto ptr = (ArrayField*)t->getField("bounded_array");
        CHECK(ptr->size_if_bounded_array() == 10);
        for (uint64_t idx = 0, N = ptr->size_if_bounded_array(); idx < N; idx++) {
            CHECK(*ptr->at(idx)->value<double>(&tv) == tv.bounded_array.at(idx));
        }
    }

    SECTION("array: update")     {
        auto ptr = (ArrayField*)t->getField("bounded_array");
        for (uint64_t idx = 0, N = ptr->size_if_bounded_array(); idx < N; idx++) {
            *ptr->at(idx)->value<double>(&tv) = idx*20.0+1.0;
        }
        for (uint64_t idx = 0, N = ptr->size_if_bounded_array(); idx < N; idx++) {
            CHECK(*ptr->at(idx)->value<double>(&tv) == idx*20.0+1.0);
            CHECK(*ptr->at(idx)->value<double>(&tv) == tv.bounded_array.at(idx)); // Updating the original data structure
        }
    }
}