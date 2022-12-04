#pragma once

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json {

    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node final
        : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
    public:
        using variant::variant;
        using Value = variant;

        Node(Value value) : std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>(value) {

        }

        bool IsInt() const;
        bool IsPureDouble() const;
        bool IsDouble() const;
        bool IsBool() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsString() const;
        bool IsMap() const;

        const Array& AsArray() const;
        bool AsBool() const;
        double AsDouble() const;
        int AsInt() const;
        Array& AsArrayNotConst();


        const std::string& AsString() const;

        bool IsDict() const;
        const Dict& AsDict() const;
        Dict& AsDictNotConst();

        bool operator==(const Node& rhs) const;

        const Dict& AsMap() const;

        const Value& GetValue() const;
        Value& GetValueNotConst();

    };

    inline bool operator!=(const Node& lhs, const Node& rhs);

    class Document {
    public:
        explicit Document(Node root)
            : root_(std::move(root)) {
        }
        Document() {};
        const Node& GetRoot() const {
            return root_;
        }

    private:
        Node root_;
    };

    inline bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.GetRoot() == rhs.GetRoot();
    }

    inline bool operator!=(const Document& lhs, const Document& rhs) {
        return !(lhs == rhs);
    }

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

}  // namespace json