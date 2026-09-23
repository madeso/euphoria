#include "eu/kdl/kdl.h"

#include <cassert>
#include <kdl/kdl.h>

namespace eu::kdl
{


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// internal helper functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    std::string_view to_u8string_view(kdl_str const& s)
    {
        return std::string_view{reinterpret_cast<char const*>(s.data), s.len};
    }

    kdl_str to_kdl_str(std::string_view s)
    {
        return kdl_str{.data = (s.data()), .len = s.size()};
    }

    std::variant<integer_type, float_type, std::string> kdl_number_to_variant(kdl_number const& n)
    {
        switch (n.type) {
        case KDL_NUMBER_TYPE_INTEGER:
            return n.integer;
        case KDL_NUMBER_TYPE_FLOATING_POINT:
            return n.floating_point;
        case KDL_NUMBER_TYPE_STRING_ENCODED:
            return std::string{to_u8string_view(n.string)};
        default:
            assert(false && "invalid kdl_number");
            return 0;
        }
    }

    std::variant<std::monostate, bool, Number, std::string> kdl_value_to_variant(kdl_value const& val)
    {
        switch (val.type) {
        case KDL_TYPE_NULL:
            return std::monostate{};
        case KDL_TYPE_BOOLEAN:
            return val.boolean;
        case KDL_TYPE_NUMBER:
            return Number{val.number};
        case KDL_TYPE_STRING:
            return std::string{to_u8string_view(val.string)};
        default:
            assert(false &&  "invalid kdl_value");
            return std::monostate{};
        }
    }

    [[nodiscard]]
    bool emit_nodes(kdl_emitter* emitter, std::vector<Node> const& nodes)
    {
        for (const auto& node : nodes)
        {
            if (node.type_annotation.has_value())
            {
                if (!kdl_emit_node_with_type(
                    emitter, to_kdl_str(*node.type_annotation), to_kdl_str(node.name)))
                {
                    return false;
                }
            } else {
                if (!kdl_emit_node(emitter, to_kdl_str(node.name)))
                {
                    return false;
                }
            }

            for (const auto& arg : node.args)
            {
                auto v = static_cast<kdl_value>(arg);
                if (!kdl_emit_arg(emitter, &v))
                {
                    return false;
                }
            }

            for (const auto& [key, value] : node.properties)
            {
                auto v = static_cast<kdl_value>(value);
                if (!kdl_emit_property(emitter, to_kdl_str(key), &v))
                {
                    return false;
                }
            }

            if (!node.children.empty())
            {
                if (!kdl_start_emitting_children(emitter))
                {
                    return false;
                }
                if (emit_nodes(emitter, node.children) == false)
                {
                    return false;
                }
                if (!kdl_finish_emitting_children(emitter))
                {
                    return false;
                }
            }
        }

        return true;
    }

    template<typename TDefer>
    struct Defer
    {
        TDefer t;

        explicit Defer(TDefer&& a)
            : t(std::move(a))
        {}

        ~Defer()
        {
            t();
        }

        Defer() = delete;

        Defer(const Defer&) = delete;
        Defer(Defer&&) = delete;

        Defer operator=(const Defer&) = delete;
        Defer operator=(Defer&&) = delete;
    };
} // namespace



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Number
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Number::Number()
    : m_value{ 0ll }
{}

Number::Number(long long n)
    : m_value{ n }
{}

Number::Number(long n)
    : m_value{ static_cast<integer_type>(n) }
{}

Number::Number(int n)
    : m_value{ static_cast<integer_type>(n) }
{}

Number::Number(short n)
    : m_value{ static_cast<integer_type>(n) }
{}

Number::Number(double n)
    : m_value{ n }
{}

Number::Number(float n)
    : m_value{ static_cast<float_type>(n) }
{}

Number::Number(kdl_number const& n)
    : m_value{ kdl_number_to_variant(n) }
{}

NumberType Number::type() const noexcept
{
    return static_cast<NumberType>(m_value.index());
}

Number::operator kdl_number() const
{
    kdl_number result;
    std::visit(
        [&result]<typename T0>(const T0& n) {
            using T = std::decay_t<T0>;
            if constexpr (std::is_same_v<T, integer_type>) {
                result.type = KDL_NUMBER_TYPE_INTEGER;
                result.integer = n;
            } else if constexpr (std::is_same_v<T, float_type>) {
                result.type = KDL_NUMBER_TYPE_FLOATING_POINT;
                result.floating_point = n;
            } else if constexpr (std::is_same_v<T, std::string>) {
                result.type = KDL_NUMBER_TYPE_STRING_ENCODED;
                result.string = to_kdl_str(n);
            } else {
                assert(false && "incomplete visit");
                result.type = KDL_NUMBER_TYPE_INTEGER;
                result.integer = 0;
            }
        },
        m_value);
    return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HasTypeAnnotation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HasTypeAnnotation::HasTypeAnnotation(std::string_view t)
    : type_annotation{ t }
{}

void HasTypeAnnotation::remove_type_annotation()
{
    type_annotation.reset();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Value
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Value::Value(bool b)
    : m_value{ b }
{}

Value::Value(std::string_view s)
    : m_value{ std::string{s} }
{}

Value::Value(std::string s)
    : m_value{ std::move(s) }
{}

Value::Value(char const* s)
    : m_value{ std::string{s} }
{}

Value::Value(Number n)
    : m_value{ std::move(n) }
{}

Value::Value(std::string_view ta, bool b)
    : HasTypeAnnotation{ ta }
    , m_value{ b }
{}

Value::Value(std::string_view ta, std::string_view s)
    : HasTypeAnnotation{ ta }
    , m_value{ std::string{s} }
{}

Value::Value(std::string_view ta, std::string s)
    : HasTypeAnnotation{ ta },
    m_value{ std::move(s) }
{}

Value::Value(std::string_view ta, Number n)
    : HasTypeAnnotation{ ta },
    m_value{ std::move(n) }
{}

Value::Value(kdl_value const& val)
    : m_value(kdl_value_to_variant(val))
{
    if (val.type_annotation.data != nullptr) {
        type_annotation = to_u8string_view(val.type_annotation);
    }
}

Value& Value::operator=(bool b)
{
    m_value = b;
    return *this;
}

Value& Value::operator=(std::string_view s)
{
    m_value = std::string{ s };
    return *this;
}

Value& Value::operator=(std::string s)
{
    m_value = std::move(s);
    return *this;
}

Value& Value::operator=(Number const& n)
{
    m_value = n;
    return *this;
}

Value& Value::operator=(Number&& n)
{
    m_value = std::move(n);
    return *this;
}

void Value::set_to_null()
{
    m_value = std::monostate{};
}

Type Value::type() const noexcept
{
    return static_cast<Type>(m_value.index());
}

const Number& Value::as_number() const
{
    if (std::holds_alternative<Number>(m_value)) {
        return std::get<Number>(m_value);
    }
    else {
        static const Number missing_number = 0;
        return missing_number;
    }
}

const std::string& Value::as_string() const
{
    if (std::holds_alternative<std::string>(m_value)) {
        return std::get<std::string>(m_value);
    }
    else {
        static const std::string missing_string = "";
        return missing_string;
    }
}

bool Value::as_bool() const
{
    if (std::holds_alternative<bool>(m_value)) {
        return std::get<bool>(m_value);
    }
    else {
        return false;
    }
}

bool Value::is_null() const
{
    return std::holds_alternative<std::monostate>(m_value);
}

Value::operator kdl_value() const
{
    kdl_value result;

    std::visit([&result](const auto& v)
        {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, bool>)
            {
                result.type = KDL_TYPE_BOOLEAN;
                result.boolean = v;
            } else if constexpr (std::is_same_v<T, Number>)
            {
                result.type = KDL_TYPE_NUMBER;
                result.number = static_cast<kdl_number>(v);
            } else if constexpr (std::is_same_v<T, std::string>)
            {
                result.type = KDL_TYPE_STRING;
                result.string = to_kdl_str(v);
            } else {
                result.type = KDL_TYPE_NULL;
            }
        },
        m_value);
    
    if (type_annotation.has_value())
    {
        result.type_annotation = to_kdl_str(*type_annotation);
    }
    else
    {
        result.type_annotation = {.data = nullptr, .len = 0};
    }
    
    return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Node
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Node::Node(std::string_view name)
    : name{ name }
{}

Node::Node(std::string_view ta, std::string_view name)
    : HasTypeAnnotation{ ta }
    , name{ name }
{}

Node::Node(std::string_view name, std::vector<Value> args, std::map<std::string, Value, std::less<>> properties, std::vector<Node> children)
    : name{ name }
    , args{ std::move(args) }
    , properties{ std::move(properties) }
    , children{ std::move(children) }
{}

Node::Node(std::string_view ta, std::string_view name, std::vector<Value> args, std::map<std::string, Value, std::less<>> properties, std::vector<Node> children)
    : HasTypeAnnotation{ ta }
    , name{ name }
    , args{ std::move(args) }
    , properties{ std::move(properties) }
    , children{ std::move(children) } 
{}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Document
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Document::Document(std::vector<Node> nodes)
    : nodes{ std::move(nodes) }
{}

Document::Document(std::initializer_list<Node> nodes)
    : nodes{ nodes }
{}

std::optional<std::string> Document::to_string() const
{
    return to_string(Version::kdl_1);
}

std::optional<std::string> Document::to_string(Version version) const
{
    kdl_emitter_options opts = KDL_DEFAULT_EMITTER_OPTIONS;

    if (version == Version::kdl_1) opts.version = KDL_VERSION_1;
    if (version == Version::kdl_2) opts.version = KDL_VERSION_2;

    kdl_emitter* emitter = kdl_create_buffering_emitter(&opts);
    if (emitter == nullptr)
    {
        assert(false && "Error initializing the KDL emitter");
        return std::nullopt;
    }
    auto defer_emitter = Defer{[&]{ kdl_destroy_emitter(emitter); }};
    
    if (false == emit_nodes(emitter, nodes))
    {
        return std::nullopt;
    }

    auto result = std::string{to_u8string_view(kdl_get_emitter_buffer(emitter))};
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DocumentResult
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


DocumentResult::DocumentResult(Document doc)
    : document{ std::move(doc) }
{}

DocumentResult::DocumentResult(std::string err)
    : error{ std::move(err) }
{}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Parsing
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{

    DocumentResult Document_read_from(kdl_parser* parser)
    {
        Document doc;
        auto* node_list = &doc.nodes;
        Node* current_node = nullptr;
        std::vector<Node*> stack;

        while (true)
        {
            auto* ev = kdl_parser_next_event(parser);

            switch (ev->event)
            {
            case KDL_EVENT_EOF:
                return DocumentResult{ doc };
            case KDL_EVENT_PARSE_ERROR:
                return DocumentResult{ static_cast<std::string>(to_u8string_view(ev->value.string)) };
            case KDL_EVENT_START_NODE:
            {
                auto name = to_u8string_view(ev->name);
                if (ev->value.type_annotation.data != nullptr)
                {
                    auto ta = to_u8string_view(ev->value.type_annotation);
                    current_node = &node_list->emplace_back(ta, name);
                }
                else
                {
                    current_node = &node_list->emplace_back(name);
                }
                node_list = &current_node->children;
                stack.push_back(current_node);
                break;
            }
            case KDL_EVENT_END_NODE:
                stack.pop_back();
                if (stack.empty())
                {
                    current_node = nullptr;
                    node_list = &doc.nodes;
                }
                else
                {
                    current_node = stack.back();
                    node_list = &current_node->children;
                }
                break;
            case KDL_EVENT_ARGUMENT:
                current_node->args.emplace_back(ev->value);
                break;
            case KDL_EVENT_PROPERTY:
                current_node->properties[std::string{ to_u8string_view(ev->name) }] = Value{ ev->value };
                break;
            case KDL_EVENT_COMMENT:
                // ignore comments
                break;
            default:
                assert(false && "Invalid event from kdl_parser");
                break;
            }
        }
    }
}


DocumentResult parse(std::string_view kdl_text)
{
    return parse(kdl_text, Version::any);
}

DocumentResult parse(std::string_view kdl_text, Version version)
{
    kdl_parse_option opts = KDL_DEFAULTS;
    
    switch (version)
    {
    case Version::kdl_1:
        opts = KDL_READ_VERSION_1;
        break;
    case Version::kdl_2:
        opts = KDL_READ_VERSION_2;
        break;
    case Version::any:
        {
            auto ret = parse(kdl_text, Version::kdl_2);
            if (ret.document.has_value())
            {
                return ret;
            }
            else
            {
                return parse(kdl_text, Version::kdl_1);
            }
        }

    default:
        assert(false && "unhandled case");
        break;
    }

    const kdl_str text = {.data = (kdl_text.data()), .len = kdl_text.size()};
    kdl_parser* parser = kdl_create_string_parser(text, opts);
    if (parser == nullptr)
    {
        assert(false && "Error initializing the KDL parser");
        return DocumentResult("internal error");
    }
    auto defer_parser = Defer{[&]{ kdl_destroy_parser(parser); }};

    auto doc = Document_read_from(parser);
    return doc;
}

}
