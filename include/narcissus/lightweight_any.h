// lightweight_any.h
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

#ifndef NARCISSUS_LIGHTWEIGHT_ANY_H
#define NARCISSUS_LIGHTWEIGHT_ANY_H

#include <typeindex>
#include <memory>
#include <cassert>

struct lightweight_any {
    template<typename T,
          std::enable_if_t<std::is_pointer<T>::value, bool> = false>
    lightweight_any ( T* obj) : idx(typeid(T)), ptr{(void*)obj} {
    }

    template<typename T>
    lightweight_any ( T& obj) : idx(typeid(T)), ptr{(void*)&obj} {
    }

    template<typename T>
    lightweight_any ( std::unique_ptr<T>& obj)  : idx(typeid(T)), ptr{(void*)obj.get()} {
    }

    template<typename T>
    lightweight_any ( std::shared_ptr<T>& obj)  : idx(typeid(T)), ptr{(void*)obj.get()} {
    }

    template<typename T>
    lightweight_any ( std::weak_ptr<T>& obj)  : idx(typeid(T)), ptr{(void*)obj.get()} {
    }

    template<typename T>
    lightweight_any (const T* obj) : idx(typeid(T)), ptr{(void*)&obj} {
    }

    template<typename T,
          std::enable_if_t<std::is_pointer<T>::value, bool> = false>
    lightweight_any (const T& obj) : idx(typeid(T)), ptr{(void*)&obj} {
    }

    template<typename T>
    lightweight_any (const std::unique_ptr<T>& obj)  : idx(typeid(T)), ptr{(void*)obj.get()} {
    }

    template<typename T>
    lightweight_any (const std::shared_ptr<T>& obj)  : idx(typeid(T)), ptr{(void*)obj.get()} {
    }

    template<typename T>
    lightweight_any(const std::weak_ptr<T>& obj) : idx{typeid(T)}, ptr{(void*)obj.get()} {

    }


    void* raw() const;

    template<typename T> T* get() const {
        assert(typeid(T) == idx);
        return (T*)ptr;
    }

    void* raw();

    template<typename T> T* get() {
        assert(typeid(T) == idx);
        return (T*)ptr;
    }

    lightweight_any();

    lightweight_any(const lightweight_any&) = default;
    lightweight_any(lightweight_any&) = default;
    lightweight_any& operator=(const lightweight_any&) = default;
    lightweight_any& operator=(lightweight_any&) = default;

protected:
    std::type_index idx;
    void* ptr;
};

#endif //NARCISSUS_LIGHTWEIGHT_ANY_H