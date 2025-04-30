/*
Program 1:
Explanation
1.	Serialization:
•	The serialize function uses std::tie to access the fields of the struct.
•	The std::apply function applies the serializeField function to each field.
2.	Deserialization:
•	The deserialize function uses std::tie to access the fields of the struct.
•	The std::apply function applies the deserializeField function to each field.
3.	Specialization for std::string:
•	Special handling is provided for std::string to ensure proper serialization and deserialization.
4.	Main Function:
•	Demonstrates serialization and deserialization of the msg struct.

Example Input:
msg originalMsg = {1, 42.5, "example"};

Output:
Serialized Data: 1 42.5 "example" 
Deserialized Data: id=1, value=42.5, name=example

*/




#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>


struct msg {
    int id;
    double value;
    std::string name;
};

// Helper function to serialize a single field
template <typename T>
void serializeField(std::ostream& os, const T& field) {
    os << field << " ";
}

// Specialization for std::string
template <>
void serializeField<std::string>(std::ostream& os, const std::string& field) {
    os << "\"" << field << "\" ";
}

// Helper function to deserialize a single field
template <typename T>
void deserializeField(std::istream& is, T& field) {
    is >> field;
}

// Specialization for std::string
template <>
void deserializeField<std::string>(std::istream& is, std::string& field) {
    is >> std::ws; // Skip leading whitespace
    std::getline(is, field, '\"'); // Read until the next quote
}

// Serialize a struct using templates
template <typename T>
std::string serialize(const T& obj) {
    std::ostringstream os;
    const auto& fields = std::tie(obj.id, obj.value, obj.name); // Access struct fields
    std::apply([&os](const auto&... args) { ((serializeField(os, args)), ...); }, fields);
    return os.str();
}

// Deserialize a struct using templates
template <typename T>
void deserialize(const std::string& data, T& obj) {
    std::istringstream is(data);
    auto& fields = std::tie(obj.id, obj.value, obj.name); // Access struct fields
    std::apply([&is](auto&... args) { ((deserializeField(is, args)), ...); }, fields);
}

int main() {
    // Example struct
    msg originalMsg = {1, 42.5, "example"};

    // Serialize the struct
    std::string serializedData = serialize(originalMsg);
    std::cout << "Serialized Data: " << serializedData << std::endl;

    // Deserialize the struct
    msg deserializedMsg;
    deserialize(serializedData, deserializedMsg);
    std::cout << "Deserialized Data: id=" << deserializedMsg.id
              << ", value=" << deserializedMsg.value
              << ", name=" << deserializedMsg.name << std::endl;

    return 0;
}

/*
Program 2:

Explanation:

Serializable Base Class:

Defines a pure virtual interface for serialize and deserialize. Any struct or class that needs to be serializable should inherit from this class and implement these methods.
serialize_field and deserialize_field Template Functions:

These are the core template functions responsible for handling the serialization and deserialization of individual data members.
Type Traits (std::enable_if_t, std::is_arithmetic_v, std::is_enum_v, std::is_same_v, std::is_base_of_v): These are used to conditionally enable the template functions based on the type of the data member. This allows us to have different implementations for basic types, strings, and other serializable objects.
Basic Types (Arithmetic and Enums): The primary template handles arithmetic types (like int, double, char) and enums by directly writing/reading their raw bytes using os.write() and is.read().
std::string Specialization: A specialized version is provided for std::string. It first serializes/deserializes the length of the string as a uint32_t, followed by the string's data. This is necessary because strings have variable lengths.
std::vector Specializations:
One specialization handles std::vector of types that inherit from Serializable. It first serializes/deserializes the size of the vector, and then iterates through the elements, calling their respective serialize/deserialize methods.
Another specialization handles std::vector of basic types or strings. It serializes/deserializes the size and then iterates through the elements, calling the appropriate serialize_field/deserialize_field for the element type.
InnerMsg and msg Structs:

These are example structs that inherit from Serializable.
They implement the serialize and deserialize methods by calling the appropriate serialize_field and deserialize_field template functions for each of their data members. The order of serialization and deserialization must be the same.
save_to_file and load_from_file Template Functions:

These are utility template functions that take a serializable object and a filename.
save_to_file opens an output file stream in binary mode and calls the object's serialize method.
load_from_file opens an input file stream in binary mode and calls the object's deserialize method.
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <type_traits> // For type traits

// Forward declaration
struct msg;

// Base class for serialization/deserialization
class Serializable {
public:
    virtual ~Serializable() = default;
    virtual void serialize(std::ostream& os) const = 0;
    virtual void deserialize(std::istream& is) = 0;
};

// Template function for serializing basic types
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T> || std::is_same_v<std::string, std::decay_t<T>>, void>
serialize_field(std::ostream& os, const T& value) {
    os.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

// Template function for deserializing basic types
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T> || std::is_same_v<std::string, std::decay_t<T>>, void>
deserialize_field(std::istream& is, T& value) {
    is.read(reinterpret_cast<char*>(&value), sizeof(value));
}

// Specialization for std::string serialization
template <>
void serialize_field(std::ostream& os, const std::string& value) {
    uint32_t length = value.length();
    os.write(reinterpret_cast<const char*>(&length), sizeof(length));
    os.write(value.data(), length);
}

// Specialization for std::string deserialization
template <>
void deserialize_field(std::istream& is, std::string& value) {
    uint32_t length;
    is.read(reinterpret_cast<char*>(&length), sizeof(length));
    value.resize(length);
    is.read(&value[0], length);
}

// Template function for serializing vectors of serializable types
template <typename T>
typename std::enable_if_t<std::is_base_of_v<Serializable, std::decay_t<T>>, void>
serialize_field(std::ostream& os, const std::vector<T>& vec) {
    uint32_t size = vec.size();
    os.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (const auto& item : vec) {
        item.serialize(os);
    }
}

// Template function for deserializing vectors of serializable types
template <typename T>
typename std::enable_if_t<std::is_base_of_v<Serializable, std::decay_t<T>>, void>
deserialize_field(std::istream& is, std::vector<T>& vec) {
    uint32_t size;
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    vec.resize(size);
    for (auto& item : vec) {
        item.deserialize(is);
    }
}

// Template function for serializing vectors of basic types/strings
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T> || std::is_same_v<std::string, std::decay_t<T>>, void>
serialize_field(std::ostream& os, const std::vector<T>& vec) {
    uint32_t size = vec.size();
    os.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (const auto& item : vec) {
        serialize_field(os, item);
    }
}

// Template function for deserializing vectors of basic types/strings
template <typename T>
typename std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T> || std::is_same_v<std::string, std::decay_t<T>>, void>
deserialize_field(std::istream& is, std::vector<T>& vec) {
    uint32_t size;
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    vec.resize(size);
    for (auto& item : vec) {
        deserialize_field(is, item);
    }
}

// Example struct msg
struct InnerMsg : public Serializable {
    int id;
    std::string name;

    InnerMsg() : id(0), name("") {}
    InnerMsg(int i, const std::string& n) : id(i), name(n) {}

    void serialize(std::ostream& os) const override {
        serialize_field(os, id);
        serialize_field(os, name);
    }

    void deserialize(std::istream& is) override {
        deserialize_field(is, id);
        deserialize_field(is, name);
    }

    bool operator==(const InnerMsg& other) const {
        return id == other.id && name == other.name;
    }
};

struct msg : public Serializable {
    int version;
    double price;
    std::string symbol;
    std::vector<int> flags;
    InnerMsg inner;
    std::vector<InnerMsg> inner_vec;

    msg() : version(0), price(0.0), symbol(""), inner() {}
    msg(int v, double p, const std::string& s, const std::vector<int>& f, const InnerMsg& in, const std::vector<InnerMsg>& iv)
        : version(v), price(p), symbol(s), flags(f), inner(in), inner_vec(iv) {}

    void serialize(std::ostream& os) const override {
        serialize_field(os, version);
        serialize_field(os, price);
        serialize_field(os, symbol);
        serialize_field(os, flags);
        serialize_field(os, inner);
        serialize_field(os, inner_vec);
    }

    void deserialize(std::istream& is) override {
        deserialize_field(is, version);
        deserialize_field(is, price);
        deserialize_field(is, symbol);
        deserialize_field(is, flags);
        deserialize_field(is, inner);
        deserialize_field(is, inner_vec);
    }

    bool operator==(const msg& other) const {
        return version == other.version &&
               price == other.price &&
               symbol == other.symbol &&
               flags == other.flags &&
               inner == other.inner &&
               inner_vec == other.inner_vec;
    }
};

// Template functions for saving and loading serializable objects
template <typename T>
typename std::enable_if_t<std::is_base_of_v<Serializable, std::decay_t<T>>, bool>
save_to_file(const T& obj, const std::string& filename) {
    std::ofstream ofs(filename, std::ios::binary);
    if (ofs.is_open()) {
        obj.serialize(ofs);
        return true;
    }
    return false;
}

template <typename T>
typename std::enable_if_t<std::is_base_of_v<Serializable, std::decay_t<T>>, bool>
load_from_file(T& obj, const std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (ifs.is_open()) {
        obj.deserialize(ifs);
        return true;
    }
    return false;
}

int main() {
    InnerMsg inner1(10, "Inner A");
    InnerMsg inner2(20, "Inner B");
    msg my_msg(1, 123.45, "AAPL", {1, 2, 3}, inner1, {inner1, inner2});

    // Serialize to a file
    if (save_to_file(my_msg, "my_msg.bin")) {
        std::cout << "Message serialized successfully to my_msg.bin" << std::endl;
    } else {
        std::cerr << "Failed to serialize message." << std::endl;
        return 1;
    }

    // Deserialize from the file
    msg loaded_msg;
    if (load_from_file(loaded_msg, "my_msg.bin")) {
        std::cout << "Message deserialized successfully from my_msg.bin" << std::endl;
        if (my_msg == loaded_msg) {
            std::cout << "Loaded message is identical to the original." << std::endl;
        } else {
            std::cout << "Loaded message is different from the original." << std::endl;
            std::cout << "Original: version=" << my_msg.version << ", price=" << my_msg.price << ", symbol=" << my_msg.symbol << ", flags=[";
            for (int f : my_msg.flags) std::cout << f << " ";
            std::cout << "], inner.id=" << my_msg.inner.id << ", inner.name=" << my_msg.inner.name << ", inner_vec.size=" << my_msg.inner_vec.size() << std::endl;
            std::cout << "Loaded:   version=" << loaded_msg.version << ", price=" << loaded_msg.price << ", symbol=" << loaded_msg.symbol << ", flags=[";
            for (int f : loaded_msg.flags) std::cout << f << " ";
            std::cout << "], inner.id=" << loaded_msg.inner.id << ", inner.name=" << loaded_msg.inner.name << ", inner_vec.size=" << loaded_msg.inner_vec.size() << std::endl;
        }
    } else {
        std::cerr << "Failed to deserialize message." << std::endl;
        return 1;
    }

    return 0;
}

