#include "json_builder.h"

namespace json {

    KeyContext Builder::Key(std::string key) {
        BuilderContext temp(*this);
        if (double_key) {
            throw LogicError("error");
        }
        if (nodes_stack_.back().IsDict()) {
            nodes_key_.push_back(&nodes_stack_.back().AsDictNotConst().operator[](key));
            ++key_no_in_dict;
        }
        else {
            throw LogicError("error");
        }
        double_key = true;
        return temp;
    }

    ValueContext Builder::Value(Node::Value value) {
        BuilderContext temp(*this);
        if (state_dict && !state_array) {
            --key_no_in_dict;
            if (key_no_in_dict < 0) {
                throw LogicError("error");
            }
        }
        if (end_node) {
            throw LogicError("error");
        }
        if (nodes_stack_.size() == 0) {
            end_node = true;
        }
        double_key = false;
        Node p(value);
        if (nodes_stack_.size() != 0) {
            if (nodes_stack_.back().IsDict()) {
                if (nodes_key_.size() != 0) {
                    *nodes_key_.back() = p;
                    nodes_key_.pop_back();
                }
            }
            if (nodes_stack_.back().IsArray()) {
                nodes_stack_.back().AsArrayNotConst().push_back(p);
            }
        }
        else {
            nodes_stack_.push_back(p);
        }

        return  temp;
    }

    Node Builder::Build() {
        if (nodes_stack_.size() > 1 || nodes_stack_.size() == 0) {
            throw LogicError("error");
        }
        if (opend_dict.size() != 0 || opend_arr.size() != 0) {
            throw LogicError("error");
        }
        return nodes_stack_[0];
    }

    DictItemContext Builder::StartDict() {

        BuilderContext temp(*this);

        if (state_dict && !state_array) {
            --key_no_in_dict;
            if (key_no_in_dict < 0) {
                throw LogicError("error");
            }
        }
        state_dict = true;
        if (end_node) {
            throw LogicError("error");
        }

        double_key = false;
        Dict dic;
        opend_dict.push_back(1);
        Node node = Node(dic);
        nodes_stack_.push_back(node);
        return temp;
    }

    EndDictContext Builder::EndDict() {

        BuilderContext temp(*this);
        state_dict = false;
        double_key = false;

        if (opend_dict.empty()) {
            throw LogicError("error");
        }
        opend_dict.pop_back();
        if (nodes_stack_.size() == 0) {
            throw LogicError("error");
        }
        if (nodes_stack_.size() != 1) {
            Node temp(nodes_stack_.back());
            if (!nodes_stack_.back().IsDict())
                throw LogicError("error");
            nodes_stack_.pop_back();
            if (nodes_stack_.back().IsDict()) {
                *nodes_key_.back() = temp;
                nodes_key_.pop_back();
            }
            else if (nodes_stack_.back().IsArray()) {
                nodes_stack_.back().AsArrayNotConst().push_back(temp);
            }

        }
        else if (nodes_stack_.size() == 1) {
            end_node = true;
        }
        return temp;
    }

    StartArrayContext Builder::StartArray() {

        DictItemContext temp(*this);
        if (state_dict && !state_array) {
            --key_no_in_dict;
            if (key_no_in_dict < 0) {
                throw LogicError("error");
            }
        }
        state_array = true;
        if (end_node) {
            throw LogicError("error");
        }
        double_key = false;
        Array ar;
        opend_arr.push_back(1);
        Node node = Node(ar);
        nodes_stack_.push_back(node);
        return temp;
    }

    EndArrayContext Builder::EndArray() {
        DictItemContext temp(*this);
        if (state_dict && !state_array) {
            --key_no_in_dict;
            if (key_no_in_dict < 0) {
                throw LogicError("error");
            }
        }
        state_array = false;
        double_key = false;
        if (opend_arr.empty()) {
            throw LogicError("error");
        }
        opend_arr.pop_back();
        if (nodes_stack_.size() == 0) {
            throw LogicError("error");
        }
        if (nodes_stack_.size() != 1) {
            Node temp(nodes_stack_.back());
            if (nodes_stack_.back().IsDict())
                throw LogicError("error");
            nodes_stack_.pop_back();
            if (nodes_stack_.back().IsDict()) {
                if (nodes_key_.size() != 0) {
                    *nodes_key_.back() = temp;
                    nodes_key_.pop_back();
                }
            }
            else if (nodes_stack_.back().IsArray()) {
                nodes_stack_.back().AsArrayNotConst().push_back(temp);
            }
        }
        else if (nodes_stack_.size() == 1) {
            end_node = true;
        }
        return temp;
    }

    KeyContext BuilderContext::Key(std::string key)
    {
        KeyContext temp(*this);
        if (builder_.double_key) {
            throw LogicError("error");
        }
        if (builder_.nodes_stack_.back().IsDict()) {

            builder_.nodes_key_.push_back(&builder_.nodes_stack_.back().AsDictNotConst().operator[](key));
            ++builder_.key_no_in_dict;

        }
        else {
            throw LogicError("error");
        }
        builder_.double_key = true;
        return temp;
    }

    ValueContext BuilderContext::Value(Node::Value value)
    {
        ValueContext temp(*this);
        if (builder_.state_dict && !builder_.state_array) {
            --builder_.key_no_in_dict;
            if (builder_.key_no_in_dict < 0) {
                throw LogicError("error");
            }
        }
        if (builder_.end_node) {
            throw LogicError("error");
        }
        if (builder_.nodes_stack_.size() == 0) {
            builder_.end_node = true;
        }
        builder_.double_key = false;
        Node p(value);
        if (builder_.nodes_stack_.size() != 0) {
            if (builder_.nodes_stack_.back().IsDict()) {
                if (builder_.nodes_key_.size() != 0) {
                    *builder_.nodes_key_.back() = p;
                    builder_.nodes_key_.pop_back();

                }
            }
            if (builder_.nodes_stack_.back().IsArray()) {
                builder_.nodes_stack_.back().AsArrayNotConst().push_back(p);
            }
        }
        else {
            builder_.nodes_stack_.push_back(p);
        }

        return temp;
    }

    Node BuilderContext::Build()
    {
        if (builder_.nodes_stack_.size() > 1 || builder_.nodes_stack_.size() == 0) {
            throw LogicError("error");
        }
        if (builder_.opend_dict.size() != 0 || builder_.opend_arr.size() != 0) {
            throw LogicError("error");
        }
        return builder_.nodes_stack_[0];
    }

    DictItemContext BuilderContext::StartDict()
    {
        DictItemContext temp(*this);
        if (builder_.state_dict && !builder_.state_array) {
            --builder_.key_no_in_dict;
            if (builder_.key_no_in_dict < 0) {
                throw LogicError("error");
            }
        }
        builder_.state_dict = true;
        if (builder_.end_node) {
            throw LogicError("error");
        }

        builder_.double_key = false;
        Dict dic;
        builder_.opend_dict.push_back(1);
        Node node = Node(dic);
        builder_.nodes_stack_.push_back(node);
        return temp;
    }

    EndDictContext BuilderContext::EndDict()
    {
        EndDictContext temp(*this);
        if (builder_.opend_dict.empty()) {
            throw LogicError("error");
        }
        builder_.opend_dict.pop_back();
        if (builder_.nodes_stack_.size() == 0) {
            throw LogicError("error");
        }
        if (builder_.nodes_stack_.size() != 1) {
            Node temp(builder_.nodes_stack_.back());
            if (!builder_.nodes_stack_.back().IsDict())
                throw LogicError("error");
            builder_.nodes_stack_.pop_back();
            if (builder_.nodes_stack_.back().IsDict()) {
                *builder_.nodes_key_.back() = temp;
                builder_.nodes_key_.pop_back();
            }
            else if (builder_.nodes_stack_.back().IsArray()) {
                builder_.nodes_stack_.back().AsArrayNotConst().push_back(temp);
            }

        }
        else if (builder_.nodes_stack_.size() == 1) {
            builder_.end_node = true;
        }
        return temp;
    }

    StartArrayContext BuilderContext::StartArray()
    {
        StartArrayContext temp(*this);
        if (builder_.state_dict && !builder_.state_array) {
            --builder_.key_no_in_dict;
            if (builder_.key_no_in_dict < 0) {
                throw LogicError("error");
            }
        }
        builder_.state_array = true;
        if (builder_.end_node) {
            throw LogicError("error");
        }
        builder_.double_key = false;
        Array ar;
        builder_.opend_arr.push_back(1);
        Node node = Node(ar);
        builder_.nodes_stack_.push_back(node);
        return temp;
    }

    EndArrayContext BuilderContext::EndArray()
    {
        EndArrayContext temp(*this);

        if (builder_.state_dict && !builder_.state_array) {
            --builder_.key_no_in_dict;
            if (builder_.key_no_in_dict < 0) {
                throw LogicError("error");
            }
        }
        builder_.state_array = false;
        builder_.double_key = false;
        if (builder_.opend_arr.empty()) {
            throw LogicError("error");
        }
        builder_.opend_arr.pop_back();
        if (builder_.nodes_stack_.size() == 0) {
            throw LogicError("error");
        }
        if (builder_.nodes_stack_.size() != 1) {
            Node temp(builder_.nodes_stack_.back());
            if (builder_.nodes_stack_.back().IsDict())
                throw LogicError("error");
            builder_.nodes_stack_.pop_back();
            if (builder_.nodes_stack_.back().IsDict()) {
                if (builder_.nodes_key_.size() != 0) {
                    *builder_.nodes_key_.back() = temp;
                    builder_.nodes_key_.pop_back();
                }
            }
            else if (builder_.nodes_stack_.back().IsArray()) {
                builder_.nodes_stack_.back().AsArrayNotConst().push_back(temp);
            }
        }
        else if (builder_.nodes_stack_.size() == 1) {
            builder_.end_node = true;
        }
        return temp;
    }

    ValueKeyContext KeyContext::Value(Node::Value value)
    {
        ValueKeyContext temp(*this);
        Builder& builder_ = GetBuilder();
        if (builder_.state_dict && !builder_.state_array) {
            --builder_.key_no_in_dict;
            if (builder_.key_no_in_dict < 0) {
                throw LogicError("error");
            }
        }
        if (builder_.end_node) {
            throw LogicError("error");
        }
        if (builder_.nodes_stack_.size() == 0) {
            builder_.end_node = true;
        }
        builder_.double_key = false;
        Node p(value);
        if (builder_.nodes_stack_.size() != 0) {
            if (builder_.nodes_stack_.back().IsDict()) {
                if (builder_.nodes_key_.size() != 0) {
                    *builder_.nodes_key_.back() = p;
                    builder_.nodes_key_.pop_back();
                }
            }
            if (builder_.nodes_stack_.back().IsArray()) {
                builder_.nodes_stack_.back().AsArrayNotConst().push_back(p);
            }
        }
        else {
            builder_.nodes_stack_.push_back(p);
        }

        return temp;
    }

    ArrayValueContext ArrayValueContext::Value(Node::Value value)
    {
        ArrayValueContext temp(*this);
        Builder& builder_ = GetBuilder();
        if (builder_.state_dict && !builder_.state_array) {
            --builder_.key_no_in_dict;
            if (builder_.key_no_in_dict < 0) {
                throw LogicError("error");
            }
        }
        if (builder_.end_node) {
            throw LogicError("error");
        }
        if (builder_.nodes_stack_.size() == 0) {
            builder_.end_node = true;
        }
        builder_.double_key = false;
        Node p(value);
        if (builder_.nodes_stack_.size() != 0) {
            if (builder_.nodes_stack_.back().IsDict()) {
                if (builder_.nodes_key_.size() != 0) {
                    *builder_.nodes_key_.back() = p;
                    builder_.nodes_key_.pop_back();
                }
            }
            if (builder_.nodes_stack_.back().IsArray()) {
                builder_.nodes_stack_.back().AsArrayNotConst().push_back(p);
            }
        }
        else {
            builder_.nodes_stack_.push_back(p);
        }

        return temp;
    }

    ArrayValueContext StartArrayContext::Value(Node::Value value)
    {
        ArrayValueContext temp(*this);
        Builder& builder_ = GetBuilder();
        if (builder_.state_dict && !builder_.state_array) {
            --builder_.key_no_in_dict;
            if (builder_.key_no_in_dict < 0) {
                throw LogicError("error");
            }
        }
        if (builder_.end_node) {
            throw LogicError("error");
        }
        if (builder_.nodes_stack_.size() == 0) {
            builder_.end_node = true;
        }
        builder_.double_key = false;
        Node p(value);
        if (builder_.nodes_stack_.size() != 0) {
            if (builder_.nodes_stack_.back().IsDict()) {
                if (builder_.nodes_key_.size() != 0) {
                    *builder_.nodes_key_.back() = p;
                    builder_.nodes_key_.pop_back();
                }
            }
            if (builder_.nodes_stack_.back().IsArray()) {
                builder_.nodes_stack_.back().AsArrayNotConst().push_back(p);
            }
        }
        else {
            builder_.nodes_stack_.push_back(p);
        }

        return temp;
    }

}