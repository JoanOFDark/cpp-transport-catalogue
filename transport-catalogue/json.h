#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <algorithm>
#include <stdexcept>
#include <cstdio>
#include <iostream>
#include <stdio.h>

namespace json {

    class Node;
    // Сохраните объявления Dict и Array без изменения
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    using NodeJson = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
    inline const std::string NoneData{ "null" };
    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:
        explicit Node() = default;
        template<typename T>
        Node(T value) {
            node = value;
        }
        bool operator == (const Node& rhs) const;
        bool operator != (const Node& rhs) const;
        bool IsNull() const;
        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsString() const;
        bool IsBool() const;
        bool IsArray() const;
        bool IsMap() const;

        double AsDouble() const;
        bool AsBool() const;
        int AsInt() const;

        const Array& AsArray() const;
        const Dict& AsMap() const;
        const std::string& AsString() const;
        const  NodeJson& GetData() const;

    private:
        NodeJson node;
    };

    class Document {
    public:
        explicit Document(Node root);
        Document() {};

        bool operator == (const Document& rhs) const;
        bool operator != (const Document& rhs) const;
        const Node& GetRoot() const;

    private:
        Node root_;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

}  // namespace json