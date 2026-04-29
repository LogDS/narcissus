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

#include <narcissus/template_typing.h>
#include <typeindex>
#include <memory>
#include <utility>

template<typename T> void*  to_default(T enu ) {
    auto val = static_cast<uint64_t>(enu);
    uint64_t tmp= *((uint64_t*)(&val));
    void* result = (void*)tmp;
    return result;
}

struct lightweight_any {
    template<typename T,
  std::enable_if_t<is_actual_pointer<remove_cvref_t<T>>::value, bool> = true>
lightweight_any ( T obj) : idx(typeid(std::remove_pointer_t<remove_cvref_t<T>>)), ptr{(void*)(obj)}, is_fundamental{false} {
#ifdef DEBUG
        debug_name = idx.name();
#endif
    }
    template<typename T,
      std::enable_if_t<std::is_fundamental<T>::value && !is_actual_pointer<T>::value && !std::is_reference_v<std::remove_cv_t<T>>, bool> = true>
lightweight_any ( T obj) : idx(typeid(remove_cvref_t<T>)), ptr{(void*)(*(uint64_t*)(&obj))}, is_fundamental{true} {
#ifdef DEBUG
        debug_name = idx.name();
#endif
    }
    template<typename T,
  std::enable_if_t<std::is_enum<T>::value && !is_actual_pointer<T>::value, bool> = true>
lightweight_any ( T obj) : idx(typeid(remove_cvref_t<T>)), is_fundamental{true} {
        ptr = to_default<T>(obj);
#ifdef DEBUG
        debug_name = idx.name();
#endif
    }

    template<typename T>
    lightweight_any ( std::unique_ptr<T>& obj)  : idx(typeid(T)), ptr{(void*)obj.get()} {
#ifdef DEBUG
        debug_name = idx.name();
#endif
    }

    template<typename T>
    lightweight_any ( std::shared_ptr<T>& obj)  : idx(typeid(T)), ptr{(void*)obj.get()} {
#ifdef DEBUG
        debug_name = idx.name();
#endif
    }

    template<typename T>
    lightweight_any ( std::weak_ptr<T>& obj)  : idx(typeid(T)), ptr{(void*)obj.get()} {
#ifdef DEBUG
        debug_name = idx.name();
#endif
    }

    const bool is_null() const {
        return (ptr == nullptr) || (idx == typeid(std::nullptr_t));
    }

    template<typename T,
          std::enable_if_t<std::is_pointer<remove_cvref_t<T>>::value && std::is_same_v<remove_cvref_t<T>, typename remove_all_pointers<T>::type>, bool> = false>
    lightweight_any (const T& obj) : idx(typeid(remove_cvref<T>::type)), ptr{(void*)&obj} {
        remove_cvref<T> o;
#ifdef DEBUG
        debug_name = idx.name();
#endif
    }

    template<typename T>
    lightweight_any (const std::unique_ptr<T>& obj)  : idx(typeid(T)), ptr{(void*)obj.get()} {
#ifdef DEBUG
        debug_name = idx.name();
#endif
    }

    template<typename T>
    lightweight_any (const std::shared_ptr<T>& obj)  : idx(typeid(T)), ptr{(void*)obj.get()} {
#ifdef DEBUG
        debug_name = idx.name();
#endif
    }

    template<typename T>
    lightweight_any(const std::weak_ptr<T>& obj) : idx{typeid(T)}, ptr{(void*)obj.get()} {
#ifdef DEBUG
        debug_name = idx.name();
#endif

    }


    void* raw() const;

//     template<typename T> T* get() const {
// #ifdef DEBUG
//         auto local_name = typeid(T).name();
// #endif
//         if (std::type_index(typeid(T)) != idx)
//             return nullptr;
//         return (T*)ptr;
//     }

    void* raw();

    const bool operator== (const lightweight_any& obj) const {
        if (idx != obj.idx)
            return false;
        if (is_fundamental != obj.is_fundamental)
            return false;
        return *((uint64_t*)raw()) == *((uint64_t*)obj.raw());
    }

    // template<typename T> T* get() {
    //     if (std::type_index(typeid(T)) != idx)
    //         return nullptr;
    //     return is_fundamental ? (T *)(ptr) :(T*)ptr;
    // }

    lightweight_any();

protected:
    bool is_fundamental = false;
    std::type_index idx;
#ifdef DEBUG
    std::string debug_name;
#endif
    void* ptr;
} ;

#endif //NARCISSUS_LIGHTWEIGHT_ANY_H