// flatten_type_to_enum.h
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

#ifndef NARCISSUS_FLATTEN_TYPE_TO_ENUM_H
#define NARCISSUS_FLATTEN_TYPE_TO_ENUM_H

#include <field_reflection.hpp>
#include <magic_enum/magic_enum.hpp>
#include <narcissus/reflection/fields/EnumField.h>
#include <narcissus/reflection/fields/PtrField.h>
#include <narcissus/reflection/fields/ArrayField.h>
#include <narcissus/reflection/fields/DynamicArrayField.h>
#include <narcissus/reflection/fields/VariantField.h>

#include "ReflectionManager.h"
#include <iostream>



template <typename T> std::unique_ptr<Field> flatten_type_to_enum(uint64_t val, std::string name, std::function<lightweight_any(const lightweight_any&)> getter) {
    std::type_index type_i = std::type_index(typeid(T));
    bool cond = (name == "bounded_array");
    if constexpr (std::is_same_v<T, std::string>) {
        if (cond) {
            std::cout << name << " @ std::string" << std::endl;
        }
        return std::make_unique<Field>(type_i, val, name, getter, T_STRING, 0, 0);
    }
    if constexpr (std::is_void_v<T>) {
        if (cond) {
            std::cout << name << " @ void" << std::endl;
        }
        return std::make_unique<Field>(type_i, val, name, getter, T_VOID, 0, 0);
    }
    else if constexpr (std::is_null_pointer_v<T>) {
        if (cond) {
            std::cout << name << " @ nullptr" << std::endl;
        }
        return  std::make_unique<Field>(type_i, val, name, getter,T_NULLPTR, 0, 0);
    }
    else if constexpr (std::is_integral_v<T>) {
        if constexpr (std::is_signed_v<T>) {
            if (cond) {
                std::cout << name << " @ signed integral" << std::endl;
            }
            return  std::make_unique<Field>(type_i, val, name, getter,  T_SIGNED_INTEGRAL, 0, sizeof(T));
        } else {
            if (cond) {
                std::cout << name << " @ unsigned integral" << std::endl;
            }
            return  std::make_unique<Field>(type_i, val, name, getter,  T_U_INTEGRAL, 0, sizeof(T));
        }
    }
    else if constexpr (std::is_floating_point_v<T>) {
        if constexpr (std::is_signed_v<T>) {
            if (cond) {
                std::cout << name << " @ signed float" << std::endl;
            }
            return  std::make_unique<Field>(type_i, val, name, getter,  T_SIGNED_FLOAT, 0, sizeof(T));
        } else {
            if (cond) {
                std::cout << name << " @ unsigned float" << std::endl;
            }
            return  std::make_unique<Field>(type_i, val, name, getter,  T_U_FLOAT, 0, sizeof(T));
        }
    }
    else if constexpr (is_std_array<T>::value) {
        if (cond) {
            std::cout << name << " @ std::array" << std::endl;
        }
        std::function<std::unique_ptr<Field>(const uint64_t)> cell_getter = [val,name,getter](const uint64_t idx) {
            std::function<lightweight_any(const lightweight_any&)> intermediate = [getter,idx](const lightweight_any& val) {
                return &getter(val).get<T>()->at(idx);
            };
            return flatten_type_to_enum<typename remove_arg<T>::type>(val, name+"["+std::to_string(idx)+"]", intermediate);
        };
        return  std::make_unique<ArrayField>(type_i, std::move(cell_getter), val, name, getter,T_STATIC_ARRAY, std_array_size<T>::size, 0);
    }
    else if constexpr (is_vector<T>::value) {
        if (cond) {
            std::cout << name << " @ std::vector" << std::endl;
        }
        std::function<uint64_t(const lightweight_any& )> size_getter = [getter](const lightweight_any& obj) {
            return getter(obj).get<T>()->size();
            // return std::any_cast<T>(getter(obj)).size();
        };
        std::function<std::unique_ptr<Field>(const uint64_t)> cell_getter = [val,name,getter](const uint64_t idx) {
            std::function<lightweight_any(const lightweight_any&)> intermediate = [getter,idx](const lightweight_any& val) {
                return getter(val).get<T>()->at(idx);
                // return std::any_cast<T>(getter(val))[idx];
            };
            return flatten_type_to_enum<typename remove_arg<T>::type>(val, name+"["+std::to_string(idx)+"]", intermediate);
        };
        return  std::make_unique<DynamicArrayField>(type_i, std::move(cell_getter), std::move(size_getter), val, name, getter,T_OTHER_ARRAY, -1, 0);
    }
    else if constexpr (is_list<T>::value) {
        if (cond) {
            std::cout << name << " @ std::list" << std::endl;
        }
        std::function<uint64_t(const lightweight_any& )> size_getter = [getter](const lightweight_any& obj) {
            return getter(obj).get<T>()->size();
            // return std::any_cast<T>(getter(obj)).size();
        };
        std::function<std::unique_ptr<Field>(const uint64_t)> cell_getter = [val,name,getter](const uint64_t idx) {
            std::function<lightweight_any(const lightweight_any&)> intermediate = [getter,idx](const lightweight_any& val) {
                auto it = std::next(getter(val).get<T>()->begin(), idx);
                // std::advance(it, idx);
                return &(*it);
            };
            return flatten_type_to_enum<typename remove_arg<T>::type>(val, name+"["+std::to_string(idx)+"]", intermediate);
        };
        return  std::make_unique<DynamicArrayField>(type_i, std::move(cell_getter), std::move(size_getter), val, name, getter,T_OTHER_ARRAY, -1, 0);
    }
    else if constexpr (std::is_array_v<T>) {
        // UNfortunately, this is not currently supported by the library,
        if constexpr (std::is_bounded_array_v<T>) {
            if (cond) {
                std::cout << name << " @ std::array(b)" << std::endl;
            }
            return  std::make_unique<Field>(type_i, val, name, getter, T_STATIC_ARRAY, std::extent_v<T>, std::remove_reference<decltype( std::declval<T>()[0] )>::type);
        } else {
            if (cond) {
                std::cout << name << " @ std::array" << std::endl;
            }
            return  std::make_unique<Field>(type_i, val, name, getter, T_OTHER_ARRAY, -1, std::remove_reference<decltype( std::declval<T>()[0] )>::type);
        }
    }
    else if constexpr (std::is_enum_v<T>) {
        if (cond) {
            std::cout << name << " @ enum" << std::endl;
        }
        auto color_entries = magic_enum::enum_entries<T>();
        std::vector<std::pair<std::string, long long>> tmp_vals;
        assert(MAGIC_ENUM_RANGE_MIN <= std::to_underlying(std::numeric_limits<T>::min()));
        assert(MAGIC_ENUM_RANGE_MAX >= std::to_underlying(std::numeric_limits<T>::min()));
        tmp_vals.reserve(color_entries.size());
        for (const auto& [enum_val, enum_string] : color_entries) {
            tmp_vals.emplace_back(std::string(enum_string), std::to_underlying(enum_val));
        }
        std::function<int64_t(const lightweight_any&)> underlying = [](const lightweight_any& val) {
          return std::to_underlying(*val.get<T>());
        };
        return std::make_unique<EnumField>(type_i, 0, name, tmp_vals, (getter), std::move(underlying), T_ENUM);
        // return  nullptr; //EnumField::from_type<T>((getter));
    }
    else if constexpr (is_tuple<T>::value) {
        if (cond) {
            std::cout << name << " @ std::tuple" << std::endl;
        }
        ReflectionManager::reflection_record_create<T>();
        return  std::make_unique<Field>(type_i, val, name, getter, T_TUPLE, 0, 0);
    }
    else if constexpr (std::is_union_v<T>) {
        if (cond) {
            std::cout << name << " @ union" << std::endl;
        }
        return  std::make_unique<Field>(type_i, val, name, getter,T_UNION, 0, 0);
    }
    else if constexpr (is_smart_pointer<T>::value) {
        if (cond) {
            std::cout << name << " @ std::(shared|unique|weak)_ptr" << std::endl;
        }
        if constexpr (is_shared<T>::value) {
            std::function<lightweight_any(const lightweight_any&)> neu_getter = [getter](const lightweight_any&val) {
                return getter(val).get<T>()->get();
            };
            std::unique_ptr<Field> original_field = flatten_type_to_enum<typename remove_arg<T>::type>(val, name, [neu_getter](const lightweight_any& val) {
                return neu_getter(val).get<typename remove_arg<T>::type>();
                // return *std::any_cast<T*>(neu_getter(val));
            });
            return  std::make_unique<PtrField>(type_i, std::move(original_field), val, name, neu_getter,T_POINTER, 0, 0);
        } else if constexpr (is_weak<T>::value) {
            std::function<lightweight_any(const lightweight_any&)> neu_getter = [getter](const lightweight_any&val) {
                return getter(val).get<T>()->get();
                // return std::any_cast<T>(getter(val)).get();
            };
            std::unique_ptr<Field> original_field = flatten_type_to_enum<typename remove_arg<T>::type>(val, name, [neu_getter](const lightweight_any& val) {
                return neu_getter(val).get<typename remove_arg<T>::type>();
                // return *std::any_cast<T*>(neu_getter(val));
            });
            return  std::make_unique<PtrField>(type_i, std::move(original_field), val, name, neu_getter,T_POINTER, 0, 0);
        } else if constexpr (is_unique<T>::value) {
            std::function<lightweight_any(const lightweight_any&)> neu_getter = [getter](const lightweight_any&val) {
                return getter(val).get<T>()->get();
                // return std::any_cast<T>(getter(val)).get();
            };
            std::unique_ptr<Field> original_field = flatten_type_to_enum<T::element_type>(val, name, [neu_getter](const lightweight_any& val) {
                return neu_getter(val).get<T::element_type>();
                // return *std::any_cast<T*>(neu_getter(val));
            });
            return  std::make_unique<PtrField>(type_i, std::move(original_field), val, name, neu_getter,T_POINTER, 0, 0);
        }
    }
    else if constexpr (is_variant<T>::value) {
        if (cond) {
            std::cout << name << " @ std::variant" << std::endl;
        }
        ReflectionManager::reflection_record_create<T>();
        std::function<uint64_t(const lightweight_any&)> f = [getter](const lightweight_any& val) {
            return (getter(val)).get<T>()->index();
        };
        return  std::make_unique<VariantField>(type_i, std::move(f), val, name, getter,T_VARIANT, 0, 0);
    }
    else if constexpr (std::is_function_v<T>) {
        if (cond) {
            std::cout << name << " @ std::function" << std::endl;
        }
        return  std::make_unique<Field>(type_i, val, name, getter,  T_FUNCTION, 0, 0);
    }
    else if constexpr (is_actual_pointer<T>::value) {
        if (cond) {
            std::cout << name << " @ std::another_pointer" << std::endl;
        }
        std::unique_ptr<Field> original_field = flatten_type_to_enum<typename remove_all_pointers<T>::type>(val, name, [getter](const lightweight_any& val) {
            return getter(val).get<typename remove_all_pointers<T>::type>();
            // return *std::any_cast<T*>(getter(val));
        });
        return  std::make_unique<PtrField>(type_i, std::move(original_field), val, name, getter,T_POINTER, 0, 0);
    }
    else if constexpr (std::is_class_v<T>) {
        if (cond) {
            std::cout << name << " @ clazz" << std::endl;
        }
        ReflectionManager::reflection_record_create<T>();
        return  std::make_unique<Field>(type_i, val, name, getter,T_CLASS, 0, 0);
    }
    return  std::make_unique<Field>(type_i, val, name, getter,T_UNEXPECTED, 0, 0);
}

#endif //NARCISSUS_FLATTEN_TYPE_TO_ENUM_H