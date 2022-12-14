#pragma once

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>
#include <memory>

#include "json.h"

namespace json {

    class LogicError : public std::logic_error {
    public:
        using logic_error::logic_error;
    };
    class Builder;
    class DictItemContext;
    class StartArrayContext;
    class KeyContext;
    class ValueContext;
    class ValueKeyContext;
    class EndDictContext;
    class EndArrayContext;
    class ArrayValueContext;

    class BuilderContext {
    public:
        BuilderContext(Builder& builder) :builder_(builder) {
        }

        DictItemContext StartDict();
        StartArrayContext StartArray();

        KeyContext Key(std::string key);
        ValueContext Value(Node::Value value);
        EndDictContext EndDict();

        EndArrayContext EndArray();

        Node Build();
        Builder& GetBuilder() {
            return builder_;
        }
    private:
        Builder& builder_;
    };

    class DictItemContext :public BuilderContext {
    public:
        DictItemContext(BuilderContext builder_context)
            : BuilderContext(builder_context) {}

        DictItemContext StartDict() = delete;
        StartArrayContext StartArray() = delete;
        Builder& EndArray() = delete;
        Node Build() = delete;
        Builder& Value(Node::Value value) = delete;
    };

    class StartArrayContext :public BuilderContext {
    public:
        StartArrayContext(BuilderContext builder_context) : BuilderContext(builder_context) {}

        ArrayValueContext  Value(Node::Value value);
        KeyContext Key(std::string key) = delete;
        EndDictContext EndDict() = delete;
        Node Build() = delete;

    };


    class KeyContext :public BuilderContext {
    public:
        KeyContext(BuilderContext builder_context) : BuilderContext(builder_context) {}

        ValueKeyContext Value(Node::Value value);
        KeyContext Key(std::string key) = delete;
        EndDictContext EndDict() = delete;
        EndArrayContext EndArray() = delete;


        Node Build() = delete;

    };

    class ValueContext :public BuilderContext {
    public:
        ValueContext(BuilderContext builder_context) : BuilderContext(builder_context) {}

    };


    class ArrayValueContext :public BuilderContext {
    public:
        ArrayValueContext(BuilderContext builder_context) : BuilderContext(builder_context) {}

        ArrayValueContext Value(Node::Value value);
        DictItemContext EndDict() = delete;
        KeyContext Key() = delete;

        Node Build() = delete;



    };

    class ValueKeyContext :public BuilderContext {
    public:
        ValueKeyContext(BuilderContext builder_context) : BuilderContext(builder_context) {}
        ValueContext Value(Node::Value value) = delete;

        Node Build() = delete;

        DictItemContext StartDict() = delete;
        StartArrayContext StartArray() = delete;
        EndArrayContext EndArray() = delete;


    };

    class EndDictContext :public BuilderContext {
    public:
        EndDictContext(BuilderContext builder_context) : BuilderContext(builder_context) {}
    };

    class EndArrayContext :public BuilderContext {
    public:
        EndArrayContext(BuilderContext builder_context) : BuilderContext(builder_context) {}



    };
    class Builder {
    private:

    public:

        std::vector<int> opend_dict;
        std::vector<int> opend_arr;
        std::vector<Node> nodes_stack_{};
        std::vector<Node*> nodes_key_{};
        bool double_key = false;
        bool end_node = false;
        bool state_dict = false;
        bool state_array = false;
        int key_no_in_dict = 0;


        Builder() = default;
        KeyContext Key(std::string key);
        ValueContext Value(Node::Value value);
        DictItemContext StartDict();
        StartArrayContext StartArray();
        EndDictContext EndDict();
        EndArrayContext EndArray();


        Node Build();
    };

}