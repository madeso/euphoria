#pragma once

#include <functional>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

// forward-declare the types used from the C headers as not to pollute the global namespace
typedef struct kdl_str kdl_str;
typedef struct kdl_number kdl_number;
typedef struct kdl_value kdl_value;
typedef struct _kdl_parser kdl_parser;

namespace eu::kdl
{

enum class Version
{
    kdl_1,
    kdl_2,
    any
};

template <typename T> concept arithmetic_concept = std::is_arithmetic_v<T>;

// Ways in which a KDL number may be represented in C/C++
enum class NumberType
{
    integer_type = 0,
    float_type,
    string_type
};

using integer_type = long long;
using float_type = double;
using string_type = std::string;

struct Number
{
    std::variant<integer_type, float_type, std::string> m_value;

    Number();
    Number(long long n);
    Number(long n);
    Number(int n);
    Number(short n);
    Number(double n);
    Number(float n);
    Number(const kdl_number& n);

    Number(Number const&) = default;
    Number(Number&&) = default;
    Number& operator=(Number const&) = default;
    Number& operator=(Number&&) = default;

    bool operator==(const Number&) const = default;
    bool operator!=(const Number&) const = default;

    NumberType type() const noexcept;

    // Cast the number to a fundamental arithmetic type (no bounds checking,
    // no support for strings)
    template <arithmetic_concept T>
    std::optional<T> as() const
    {
        if (std::holds_alternative<integer_type>(m_value)) {
            return static_cast<T>(std::get<integer_type>(m_value));
        } else if (std::holds_alternative<float_type>(m_value)) {
            return static_cast<T>(std::get<float_type>(m_value));
        } else {
            // string
            return std::nullopt;
        }
    }

    // Cast this C++ object to a libkdl C struct
    // Note this object may hold a pointer to our string representation
    explicit operator kdl_number() const;
};

template <typename T> concept into_number_concept = requires(T t) { Number{t}; };

// Mixin
struct HasTypeAnnotation
{
    std::optional<std::string> type_annotation;

    HasTypeAnnotation() = default;
    HasTypeAnnotation(std::string_view t);


    void remove_type_annotation();

    bool operator==(const HasTypeAnnotation&) const = default;
    bool operator!=(const HasTypeAnnotation&) const = default;
};

// KDL data types
enum class Type
{
    null,
    boolean,
    number,
    string
};

// A KDL value, possibly including a type annotation
// Analogous to kdl_value
struct Value : HasTypeAnnotation
{
    std::variant<std::monostate, bool, Number, std::string> m_value;

    Value() = default;

    Value(bool b);
    Value(std::string_view s);
    Value(std::string s);
    Value(char const* s);
    Value(Number n);
    Value(into_number_concept auto n) : m_value{Number{n}} {}

    Value(std::string_view ta, bool b);
    Value(std::string_view ta, std::string_view s);
    Value(std::string_view ta, std::string s);
    Value(std::string_view ta, Number n);
    Value(std::string_view type_annotation, into_number_concept auto n);

    Value(kdl_value const& val);

    Value(Value const&) = default;
    Value(Value&&) = default;
    Value& operator=(Value const&) = default;
    Value& operator=(Value&&) = default;

    Value& operator=(bool b);
    Value& operator=(std::string_view s);
    Value& operator=(std::string s);
    Value& operator=(Number const& n);
    Value& operator=(Number&& n);
    Value& operator=(into_number_concept auto n)
    {
        m_value = Number{n};
        return *this;
    }

    bool operator==(const Value&) const = default;
    bool operator!=(const Value&) const = default;

    void set_to_null();

    Type type() const noexcept;

    const Number& as_number() const;
    const std::string& as_string() const;
    bool as_bool() const;

    bool is_null() const;

    explicit operator kdl_value() const;
};

// A node with all its contents
struct Node : HasTypeAnnotation
{
    std::string name;
    std::vector<Value> args;
    std::map<std::string, Value, std::less<>> properties;
    std::vector<Node> children;

    Node() = default;
    Node(Node const&) = default;
    Node(Node&&) = default;

    Node(std::string_view name);
    Node(std::string_view ta, std::string_view name);
    Node(std::string_view name, std::vector<Value> args, std::map<std::string, Value, std::less<>> properties, std::vector<Node> children);
    Node(std::string_view ta, std::string_view name, std::vector<Value> args, std::map<std::string, Value, std::less<>> properties, std::vector<Node> children);

    Node& operator=(Node const&) = default;
    Node& operator=(Node&&) = default;
};

// A KDL document - consisting of several nodes.
struct Document
{
    std::vector<Node> nodes;

    Document() = default;
    Document(Document const&) = default;
    Document(Document&&) = default;

    Document(std::vector<Node> nodes);
    Document(std::initializer_list<Node> nodes);

    Document& operator=(Document const&) = default;
    Document& operator=(Document&&) = default;

    [[nodiscard]] inline auto begin() const { return nodes.begin(); }
    [[nodiscard]] inline auto begin() { return nodes.begin(); }
    [[nodiscard]] inline auto end() const { return nodes.end(); }
    [[nodiscard]] inline auto end() { return nodes.end(); }

    std::optional<std::string> to_string() const;
    std::optional<std::string> to_string(Version version) const;
};

// Exception thrown on regular KDL parsing errors
struct DocumentResult
{
    std::optional<Document> document;
    std::string error;

    explicit DocumentResult(Document doc);
    explicit DocumentResult(std::string error);
};

// Load a KDL document from string
DocumentResult parse(std::string_view kdl_text);
DocumentResult parse(std::string_view kdl_text, Version version);

} // namespace kdl
