// Pythonize.cpp
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

#include <narcissus/Pythonize.h>


    inline Pythonize::Pythonize(const lightweight_any &object, Class *refl, std::shared_ptr<Field>&fld): any{object}, refl{refl}, tmp_pointer{fld} {
}


inline Pythonize::Pythonize(const lightweight_any &object, Class *refl, Field *fld): any{object}, refl{refl}, fld{fld} {
}

std::vector<std::string> Pythonize::keys() const {
    return refl->keys();
}

bool Pythonize::is_null() const {
    return ((!any.raw()));
}

uint64_t Pythonize::size() const {
    if ((getFld()) && (refl)) {
        if (refl->type() == T_TUPLE) {
            return refl->getFieldSize();
        } else {
            return 0;
        }
    } else if (getFld()) {
        if (getFld()->type() == T_STATIC_ARRAY) {
            auto ptr = (ArrayField*)getFld();
            return ptr->size_if_bounded_array();
        } else if ((getFld()->type() == T_OTHER_ARRAY)) {
            auto ptr = (DynamicArrayField*)getFld();
            return ptr->dynamic_size(any);
        } else {
            return 0;
        }
    } else
        return 0;
}

Pythonize Pythonize::operator[](const uint64_t idx) {
    auto N = size();
    if ((N == 0) || (idx >= N))
        return {};
    if ((getFld()) && (refl)) {
        if (refl->type() == T_TUPLE) {
            auto tup_refl = refl->getField("tuple_field_"+std::to_string(idx));
            // auto obj2 = tup_refl->any_value(obj3);
            return {any, refl, (Field*)tup_refl};
        } else {
            return {};
        }
    } else if (fld) {
        if (fld->type() == T_STATIC_ARRAY) {
            auto ptr = (ArrayField*)fld;
            auto i_field = ptr->at(idx);
            auto val = i_field->any_value(any);
            return {val, i_field->getClass(), nullptr};
            // extract_information(i_field->asReflection(), obj, i_field.get(), field_name+"_"+std::to_string(idx), f);
        } else if ((fld->type() == T_OTHER_ARRAY)) {
            auto ptr = (DynamicArrayField*)fld;
            auto i_field = ptr->at(idx);
            auto val = i_field->any_value(any);
            return {val, i_field->getClass(), nullptr};
        } else {
            return {};
        }
    } else
        return {};
}

Pythonize Pythonize::operator[](const std::string &key) {
    if ((!refl) || (!refl->hasKey(key))) {
        return {};
    } else {
        auto* field_ptr = (Field*)refl->getField(key);
        if (!field_ptr) {
            return {};
        } else {
            type_cases val = field_ptr->type();
            switch (val) {
                case T_VOID:
                case T_NULLPTR:
                    return {};
                    break;

                case T_SIGNED_INTEGRAL:
                    if (field_ptr->size_type() == sizeof(char)) {
                        return {(char*)(field_ptr->any_value(any).raw())};
                    } else if (field_ptr->size_type() == sizeof(int)) {
                        return {(int*)(field_ptr->any_value(any).raw())};
                    } else if ( field_ptr->size_type() == 8) {
                        return{(int8_t*)(field_ptr->any_value(any).raw())};
                    } else if ( field_ptr->size_type() == 16) {
                        return{(int16_t*)(field_ptr->any_value(any).raw())};
                    } else if ( field_ptr->size_type() == 32) {
                        return{(int32_t*)(field_ptr->any_value(any).raw())};
                    } else if ( field_ptr->size_type() == 64) {
                        return{(int64_t*)(field_ptr->any_value(any).raw())};
                    } else
                        return {};
                    break;

                case T_U_INTEGRAL:
                    if (field_ptr->size_type() == sizeof(unsigned char)) {
                        return {(unsigned char*)(field_ptr->any_value(any).raw())};
                    } else if (field_ptr->size_type() == sizeof(unsigned int)) {
                        return {(unsigned int*)(field_ptr->any_value(any).raw())};
                    } else if ( field_ptr->size_type() == 8) {
                        return{(uint8_t*)(field_ptr->any_value(any).raw())};
                    } else if ( field_ptr->size_type() == 16) {
                        return{(uint16_t*)(field_ptr->any_value(any).raw())};
                    } else if ( field_ptr->size_type() == 32) {
                        return{(uint32_t*)(field_ptr->any_value(any).raw())};
                    } else if ( field_ptr->size_type() == 64) {
                        return{(uint64_t*)(field_ptr->any_value(any).raw())};
                    } else
                        return {};

                case T_SIGNED_FLOAT:
                    if ( field_ptr->size_type() == sizeof(float)) {
                        return{(float*)(field_ptr->any_value(any).raw())};
                    } else if ( field_ptr->size_type() == sizeof(double)) {
                        return{(double*)(field_ptr->any_value(any).raw())};
                    } else
                        return {};
                    break;

                case T_ENUM: {
                    return {field_ptr->any_value(any), field_ptr->getClass(), field_ptr};
                }

                case T_CLASS: {
                    auto rt2 = field_ptr->getClass();
                    auto obj2 = field_ptr->any_value(any);
                    if (obj2.raw() != nullptr) {
                        return {obj2, rt2, nullptr};
                    } else {
                        return {};
                    }
                }

                case T_POINTER: {
                    if (any.raw() != nullptr) {
                        auto for_ptr = (PtrField*)field_ptr;
                        return Pythonize{any, for_ptr->getOriginalField()->getClass(), (Field*)for_ptr->getOriginalField()};
                    } else {
                        return {};
                    }
                }

                case T_STRING:
                    return {(std::string*)field_ptr->any_value(any).raw()};

                case T_STATIC_ARRAY:
                case T_OTHER_ARRAY:
                case T_TUPLE: {
                    // auto obj3 = field_ptr->any_value(any);
                    auto rt2 = field_ptr->getClass();
                    return Pythonize{any, rt2, field_ptr};
                }

                case T_VARIANT: {
                    auto for_ptr = (VariantField*)field_ptr;
                    auto tmp3 = for_ptr->any_value(any);
                    auto idx = for_ptr->get_idx(any);
                    auto refl2 = for_ptr->getClass();
                    auto var = refl2->getField("variant_tag_"+std::to_string(idx))->redoMapping([](const auto& x) {return x;});
                    return Pythonize{tmp3, var->getClass(), var};
                }
                break;

                case T_U_FLOAT:
                case T_UNION:
                case T_FUNCTION:
                case T_UNEXPECTED:
                    throw std::runtime_error("Currently unsupported type!");
                    break;
            }
        }
    }
    return {};
}

Pythonize::Pythonize() : refl{nullptr}, fld{nullptr} {
}
