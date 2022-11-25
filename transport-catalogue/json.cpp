#include "json.h"

using namespace std;

namespace json {
    namespace {

        Node LoadNode(istream& input);

        Node LoadArray(istream& input) {
            Array result;
            bool error = true;
            bool error_2 = true;
            for (char c; input >> c;) {

                error = false;
                if (c == ']') {
                    error_2 = false;
                    break;
                }
                if (c != ',') {
                    input.putback(c);
                }

                result.push_back(LoadNode(input));
            }
            if (error_2) {
                throw json::ParsingError("error");
            }
            if (error) {
                throw json::ParsingError("error");
            }


            return Node(move(result));
        }

        Node LoadString(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    s.push_back(ch);
                }
                ++it;
            }

            return Node(s);
        }
        
        Node LoadDict(istream& input) {
            Dict result;
            bool error = true;
            bool error_2 = true;
            bool empt = true;

            for (char c; input >> c;) {
                error = false;

                if (c == '}') {
                    error_2 = false;
                    break;
                }

                if (c == ',') {
                    empt = false;
                    input >> c;
                }

                string key = LoadString(input).AsString();
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }

            if (error_2) {
                throw json::ParsingError("error LoadDict");
            }
            if (error) {
                throw json::ParsingError("error LoadDict");
            }
            return Node(move(result));
        }

        Node LoadNumber(std::istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return Node(std::stoi(parsed_num));
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return Node(std::stod(parsed_num));
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        Node LoadBool(istream& input) {
            std::string str;
            for (char c; input.get(c);) {
                if (c == ' ' || c == ',' || c == ']' || c == '}' || c == '\n')
                {
                    input.putback(c);
                    break;
                }
                str += c;
            }

            if (str != "true" && str != "false")
                throw json::ParsingError("error LoadBool   " + str);

            if (str == "true") {
                return Node(true);
            }
            else {
                return Node(false);
            }
            return Node(false);
        }

        Node LoadNull(istream& input) {
            string s;
            using namespace std::literals;
            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            while (true) {
                if (it == end) {
                    break;
                }
                const char ch = *it;
                if (ch == '"' || ch == ',' || ch == ']' || ch == ' ') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                }
                s += ch;
                ++it;
            }
            if (s == "null")
                return Node();
            else
            {
                throw json::ParsingError("error LoadNull  " + s);
            }
            return Node();
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;

            if (c == '[') {
                return LoadArray(input);
            }
            else if (c == '{') {
                return LoadDict(input);
            }
            else if (c == '"') {

                return LoadString(input);
            }
            else if (c == 'n') {
                input.putback(c);
                return LoadNull(input);
            }
            else if (c == 't' || c == 'f') {
                input.putback(c);
                return LoadBool(input);
            }
            else {
                input.putback(c);
                return LoadNumber(input);
            }
        }
    }  // namespace

    bool Node::operator ==(const Node& rhs) const
    {
        return this->node == rhs.node;
    }

    bool Node::operator !=(const Node& rhs) const
    {
        return this->node != rhs.node;
    }


    bool Node::IsNull() const
    {
        return std::holds_alternative<nullptr_t>(node);
    }
    bool Node::IsInt() const
    {
        return std::holds_alternative<int>(node);
    }
    bool Node::IsDouble() const
    {
        return std::holds_alternative<int>(node) || std::holds_alternative<double>(node);
    }
    bool Node::IsPureDouble() const
    {
        return std::holds_alternative<double>(node);
    }
    bool Node::IsString() const
    {
        return std::holds_alternative<string>(node);
    }
    bool Node::IsBool() const
    {
        return std::holds_alternative<bool>(node);
    }
    bool Node::IsArray() const
    {
        return std::holds_alternative<Array>(node);
    }
    bool Node::IsMap() const
    {
        return std::holds_alternative<Dict>(node);
    }

    double Node::AsDouble() const
    {
        if (!std::holds_alternative<double>(node)) {
            if (!std::holds_alternative<int>(node)) {
                throw std::logic_error("Failed to convert JSON - AsDouble()"s);
            }
            else {
                return static_cast<double>(get<int>(node));
            }
        }
        return get<double>(node);
    }
    bool Node::AsBool() const
    {
        if (!std::holds_alternative<bool>(node)) {
            throw std::logic_error("Failed to convert JSON - AsBool()"s);
        }
        return get<bool>(node);
    }
    int Node::AsInt() const
    {

        if (!std::holds_alternative<int>(node)) {
            throw std::logic_error("Failed to convert JSON - AsInt()"s);
        }
        return get<int>(node);
    }
    const Array& Node::AsArray() const
    {
        if (!std::holds_alternative<Array>(node)) {
            throw std::logic_error("Failed to convert JSON - AsArray()"s);
        }
        return get<Array>(node);
    }
    const Dict& Node::AsMap() const
    {
        if (!std::holds_alternative<Dict>(node)) {
            throw std::logic_error("Failed to convert JSON - AsMap()"s);
        }
        return get<Dict>(node);
    }
    const string& Node::AsString() const
    {
        if (!std::holds_alternative<string>(node)) {
            throw std::logic_error("Failed to convert JSON - AsString()"s);
        }
        std::string str = get<string>(node);
        return get<string>(node);
    }

    const NodeJson& Node::GetData() const
    {
        return node;
    }

    Document::Document(Node root)
        : root_(move(root)) {
    }

    bool Document::operator ==(const Document& rhs) const
    {
        return this->GetRoot() == rhs.GetRoot();
    }
    bool Document::operator !=(const Document& rhs) const
    {
        return this->GetRoot() != rhs.GetRoot();
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    struct DataPrinter {
        std::ostream& out;

        void operator()(const std::nullptr_t) const {
            out << NoneData;
        }

        void operator()(Array array_of_nodes) const {
            out << "[";
            int c = 0;
            for (auto& item : array_of_nodes) {
                if (c != 0)
                    out << ",";
                std::visit(DataPrinter{ out }, item.GetData());
                ++c;
            }
            out << "]";
        }
        void operator()(Dict dict_nodes) const {
            out << "{";
            int c = 0;
            for (auto& item : dict_nodes) {
                if (c != 0)
                    out << ", ";
                out << '"' << item.first << '"';
                out << ": ";
                std::visit(DataPrinter{ out }, item.second.GetData());
                ++c;
            }
            out << "}";
        }
        void operator()(bool value) const {
            if (value) {
                out << "true";
            }
            else {
                out << "false";
            }

        }
        void operator()(int value) const {
            out << value;
        }
        void operator()(double value) const {
            out << value;
        }
        void operator()(std::string str) const {
            out << "\"";
            for (size_t i = 0; i < str.size(); ++i) {
                if (str[i] == '\r') {
                    out << '\\' << 'r';

                }
                else
                    if (str[i] == '\t') {

                        out << '\t';
                    }
                    else
                        if (str[i] == '\n') {

                            out << '\\' << 'n';
                        }
                        else
                            if (str[i] == '\"') {
                                out << '\\';
                                out << '\"';

                            }
                            else
                                if (str[i] == '\\') {
                                    out << '\\';
                                    out << '\\';

                                }
                                else {
                                    out << str[i];
                                }
            }
            out << "\"";
        }
    };

    void Print(const Document& doc, std::ostream& output) {
        const auto data = doc.GetRoot().GetData();
        std::visit(DataPrinter{ output }, data);
    }

}  // namespace json