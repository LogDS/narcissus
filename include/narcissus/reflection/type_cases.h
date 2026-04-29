// type_cases.h
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

#ifndef NARCISSUS_TYPE_CASES_H
#define NARCISSUS_TYPE_CASES_H

enum type_cases : uint8_t {
    T_VOID = 1,
    T_NULLPTR = 2,
    T_SIGNED_INTEGRAL = 3,
    T_U_INTEGRAL = 4,
    T_SIGNED_FLOAT = 5,
    T_U_FLOAT = 6,
    T_STATIC_ARRAY = 7,
    T_OTHER_ARRAY = 8,
    T_ENUM = 9,
    T_UNION = 10,
    T_CLASS = 11,
    T_FUNCTION = 12,
    T_POINTER = 13,
    T_STRING = 14,
    T_TUPLE = 15,
    T_VARIANT = 16,
    T_UNEXPECTED = 0,
};

#endif //NARCISSUS_TYPE_CASES_H