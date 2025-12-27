cmake_minimum_required(VERSION 4.0.3)
project(narcissus)

set(CMAKE_CXX_STANDARD 26)

include_directories(submodule/reflect-cpp/include)
add_subdirectory(submodule/reflect-cpp) # Add this project as a subdirectory

set(REFLECTCPP_AVRO ON) # Optional
set(REFLECTCPP_BSON ON) # Optional
set(REFLECTCPP_CBOR ON) # Optional
set(REFLECTCPP_FLEXBUFFERS ON) # Optional
set(REFLECTCPP_MSGPACK ON) # Optional
set(REFLECTCPP_TOML ON) # Optional
set(REFLECTCPP_UBJSON ON) # Optional
set(REFLECTCPP_XML ON) # Optional
set(REFLECTCPP_YAML ON) # Optional

include_directories(include)
include_directories(submodule/field-reflection/include)
include_directories(submodule/magic_enum/include)

add_executable(reflection main.cpp
        include/narcissus/gsm/gsm.h
        include/narcissus/template_typing.h
        include/narcissus/reflection/ReflectionManagerData.h
        src/narcissus/reflection/ReflectionManagerData.cpp
        include/narcissus/reflection/ReflectionManager.h
        src/narcissus/reflection/ReflectionManager.cpp
        include/narcissus/reflection/fields/Field.h
        src/narcissus/reflection/fields/Field.cpp
        include/narcissus/reflection/type_cases.h
        include/narcissus/reflection/fields/PtrField.h
        src/narcissus/reflection/fields/PtrField.cpp
        include/narcissus/reflection/fields/EnumField.h
        src/narcissus/reflection/fields/EnumField.cpp
        include/narcissus/reflection/Reflection.h
        src/narcissus/reflection/Reflection.cpp
        include/narcissus/reflection/flatten_type_to_enum.h)
target_link_libraries(reflection PRIVATE reflectcpp) # Link against the library
