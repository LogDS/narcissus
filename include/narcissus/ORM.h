// ORM.h
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

#ifndef NARCISSUS_ORM_H
#define NARCISSUS_ORM_H


#include <rfl/json.hpp>
// #include <rfl/yaml.hpp>
#include <rfl.hpp>

#include <narcissus/gsm/gsm.h>
#include <functional>
#include <any>
#include <memory>

// #define MAGIC_ENUM_AUTO_IS_FLAGS
#include <narcissus/reflection/Reflection.h>
#include <narcissus/reflection/ReflectionManager.h>
#include <narcissus/reflection/fields/EnumField.h>
#include <narcissus/reflection/fields/PtrField.h>

enum SerializationFormat {
    JSON_FORMAT,
    YAML_FORMAT
};


class ORM {
    std::map<std::uintptr_t, uint64_t> serialization;
    std::vector<GSMObject> container;
    ReflectionManager manager;

public:
    /**
     *
     * @param format Format associated with the serialized file
     * @param _stream Stream from which the file should be read
     */
    void deserialize(SerializationFormat format, std::istream& _stream);

    /**
     *
     * @param format Serializing the populated objects into a file to disk
     * @param _stream Stream where the file should be written
     */
    void serialize(SerializationFormat format, std::ostream& _stream) {
        switch (format) {
            case JSON_FORMAT:
                rfl::json::write(container, _stream, YYJSON_READ_JSON5);
                break;
            case YAML_FORMAT:
                // rfl::yaml::write(container, _stream);
                break;
        }
    }

    /**
     *
     * @tparam T                type associated with the object
     * @param original_value    Object to be serialized and temporarily stored in the cache
     * @return                  Whether the serialization occurred for the first time for the object of interest
     */
    template<typename T> bool serialize_to_cache(const T& original_value) {
        const T* ptr = &original_value;
        return serialize_to_cache<T>(ptr);
    }

private:
    void extract_information(Reflection *rt,
                         const lightweight_any& obj,
                         const Field* field_ptr,
                         std::string field_name,
                         std::function<void(const std::string&,const std::variant<std::string,Phi>&)> f);

    bool serialize_to_cache(/*std::uintptr_t address,*/ Reflection *rt, lightweight_any obj);

    template <typename T>
    bool serialize_to_cache(const T *ptr, Reflection* rt) {
        lightweight_any obj{ptr};
        return serialize_to_cache(rt, obj);
    }

    template<typename T> bool serialize_to_cache(const T* ptr) {
        std::uintptr_t address = (uintptr_t)(void*)ptr;
        auto rt = ReflectionManager::reflection_from_type<T>();
        assert(rt != nullptr);
        return serialize_to_cache<T>(ptr, rt);
    }
};


#endif //NARCISSUS_ORM_H