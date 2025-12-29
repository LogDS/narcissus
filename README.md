
# Use case Example

```C++

enum class testing_enum : int64_t { // Enums should have an assoicated representation
    A_VAL = -20,
    B_VAL = -30,
    C_VAL = 0,
    D_VAL = 10
};

struct other_elements {
    double value;
    other_elements* self;                       // Pointers are supported
    std::shared_ptr<other_elements> othero;     // Smart pointers are supported, too!
};

struct testing {
    std::string element;
    int value;
    testing_enum values;
    testing* self;
    std::tuple<int, double, std::string> tup_tup;           // Tuples are supported
    std::variant<float, std::string, bool> var_elem;        // Variants are supported
    std::list<testing> children;                            // Data structures such as lists and vectors are supported, too
    other_elements other;
    std::array<double, 10> bounded_array;
    // double bounded_array[1];
    // int things[];       Unbounded arrays are not supported
};

int main(int argc, char* argv[]) {
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
}

```

# RTTI

[TODO] 

## Limitations

The only limitations for RTTI are related to the current limitations in the programming language itself:
 - Not correctly capturing the basic C array forces the implementer to use ```std::array<T, N> object``` rather than ```T object[N]```.
 - No reflection on the data structure methods is available.

These new developments enable C++ to have RTTI à la Java, while enabling accessing objects as it was a Python programming language

## Examples

### Java-like RTTI 

The library provides Java-like RTTI object information by registering the structures of interest. We can return a Class
object similarly to Java in the following way:

```C++
    auto t = ReflectionManager::reflection_from_type<testing>(); // Pointer to a Class object
```

The ``Class`` object associated with a type comes with the following information:

```C++

    /**
     * Provides access to the data associated with the field name and belonging to an object val
     * @tparam K            Type to be casted
     * @param name          Name of the field to be accessed
     * @param val           Type-safe void* pointer passing à la std::any for the object belonging to the current class
     * @return              If the field with the given name is present and its value holds type of K and val is an object of the current class,
     *                      then it returns the pointer to the value stored in the original data. Otherwisem nullptr is returned
     */
    template <typename K> K* getField(const std::string& name, const lightweight_any& val);

    /**
     *
     * @return the accessible fields to the class
     */
    std::vector<std::string> keys() const;

    /**
     *
     * @return the class name
     */
    const std::string getName();

    /**
     *
     * @param name Name to be checked
     * @return Whether the class contains a field with a specific name
     */
    bool hasKey(const std::string& name) const;

    /**
     *
     * @return the number of the accessible fields
     */
    const uint64_t getFieldSize() const;

    /**
     *
     * @param idx the index of the field
     * @return Returns the class of the field if within the range, and nullptr otherwise
     */
    const Field* getField(uint64_t idx) const;

    /**
     *
     * @param name the name of the field to be accessed
     * @return Returns the class of the field if within the range, and nullptr otherwise
     */
    const Field* getField(const std::string& name) const;

    /**
     *
     * @return enum-based representation of non-parametric types
     */
    type_cases type();

    /**
     *
     * @return the size of the array if this is a static array (std::array<T, N>)
     */
    uint64_t size_if_bounded_array() const;
```

Each Field provides the following information:

```C++
    /**
     *
     * @return the class-type information associated with the object
     */
    virtual Class* getClass() const;

    /**
     *
     * @return field number within a class of interest
     */
    uint64_t index() const;

    /**
     *
     * @param x   Type-safe pointer à la std::any of the class containing the field of interest
     * @return    Type-safe pointer to the object pointed by the field
     */
    virtual lightweight_any any_value(const lightweight_any&x) const;

    /**
     *
     * @tparam K  Expected C++ type associated with the object
     * @param x   Type-safe pointer à la std::any of the class containing the field of interest
     * @return    If the field associated is of the desired type
     */
    template <typename K> K* value(const lightweight_any&x) const {
        return any_value(x).get<K>();
    }

    /**
     *
     * @return Name of the field
     */
    const std::string get_field_name() const;

    /**
     *
     * @return Basic type associated with the field
     */
    type_cases type() const;
    
    /**
     * 
     * @return If this points to a native C++ type, it returns the size of the object. This is important to disambiguate 
     * the original type from the sign, real/integer, which is not sufficient to provide full type reconstruction
     */
    uint64_t size_type() const;

    /**
     *
     * @return If this points to a std::array<T,N>, this returns N
     */
    uint64_t size_if_bounded_array() const;
```

Field classes are specialized 

### Pythonization

Another misconception of C++ is that it doesn't support adequate RTTI and that it does not allow a flexible access to the 
objects as other programming languages with ducktyping such as Python would.

This project shows the opposite, by remarking how the newer C++ compiler versions enable accessing the objects as they
were dictionaries and arrays, without even bothering between the distinction between the different types of pointers.

Given the same example above, you can now access 

```C++

    Pythonize p(&tv);
    auto test = p["value"];
    std::cout << *test.get<int>() << std::endl;
    auto mondo = p["element"];
    std::cout << *mondo.get<std::string>() << std::endl;
    auto values = p["values"];
    std::cout << *values.get<std::string>() << std::endl;
    auto capovar = p["var_elem"];
    std::cout << *capovar.get<std::string>() << std::endl;
    auto bounding = p["bounded_array"];
    std::cout << bounding.size() << std::endl;
    std::cout << *bounding[9].get<double>() << std::endl;
    auto children = p["children"];
    auto child_0 = children[0];
    std::cout << *child_0["element"].get<std::string>() << std::endl;

```

**WARNING**: better dealing with smart pointers!

# Object Serialization

[TODO: the aforementioned mechanism enables for ... ]

# Supported features

- [x] Object Serialization
- [ ] Object Deserialization
- [x] Object Pythonization
- [ ] XPath Querying for the objects
- [ ] JNI Wrapper
- [ ] Pybind11 Wrapper
- [ ] Function Reflection (missing due to language limitations)
- [ ] Package crawiling (missing due to language limitations)