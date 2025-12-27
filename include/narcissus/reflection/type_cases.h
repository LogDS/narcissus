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

enum type_cases {
    T_VOID,
    T_NULLPTR,
    T_SIGNED_INTEGRAL,
    T_U_INTEGRAL,
    T_SIGNED_FLOAT,
    T_U_FLOAT,
    T_STATIC_ARRAY,
    T_OTHER_ARRAY,
    T_ENUM,
    T_UNION,
    T_CLASS,
    T_FUNCTION,
    T_POINTER,
    T_STRING,
    T_UNEXPECTED,
};

#endif //NARCISSUS_TYPE_CASES_H