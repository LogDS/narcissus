cmake_minimum_required(VERSION 4.0.3)
project(narcissus)
Include(FetchContent)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
#set(BUILD_SHARED_LIBS OFF)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.11.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)



set(CMAKE_CXX_STANDARD 26)


find_package(Arrow REQUIRED)


include_directories(submodule/reflect-cpp-dependencies/yaml/include)
include_directories(submodule/reflect-cpp-dependencies/json/src)
include_directories(submodule/reflect-cpp-dependencies/csv/cpp/src)
include_directories(submodule/reflect-cpp/include)
add_subdirectory(submodule/reflect-cpp) # Add this project as a subdirectory
add_subdirectory(submodule/reflect-cpp-dependencies/yaml/)
add_subdirectory(submodule/reflect-cpp-dependencies/json/)

#set(REFLECTCPP_AVRO ON) # Optional
#set(REFLECTCPP_BSON ON) # Optional
#set(REFLECTCPP_CBOR ON) # Optional
#set(REFLECTCPP_FLEXBUFFERS ON) # Optional
#set(REFLECTCPP_MSGPACK ON) # Optional
#set(REFLECTCPP_TOML ON) # Optional
#set(REFLECTCPP_UBJSON ON) # Optional
#set(REFLECTCPP_XML ON) # Optional
#set(REFLECTCPP_YAML ON) # Optional

include_directories(include)
include_directories(submodule/field-reflection/include)
include_directories(submodule/magic_enum/include)

add_library(narcissus         include/narcissus/gsm/gsm.h
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
        include/narcissus/reflection/flatten_type_to_enum.h
        include/narcissus/reflection/fields/ArrayField.h
        src/narcissus/reflection/fields/VariantField.cpp
        src/narcissus/reflection/fields/ArrayField.cpp
        include/narcissus/lightweight_any.h
        src/narcissus/lightweight_any.cpp)

add_executable(reflection main.cpp)
target_link_libraries(reflection PRIVATE narcissus Arrow::arrow_static  yaml-cpp yyjson reflectcpp) # Link against the library

add_executable(tests tests/basic_tests.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain narcissus yaml-cpp yyjson reflectcpp)
