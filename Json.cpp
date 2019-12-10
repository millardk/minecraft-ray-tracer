//
// Created by Keegan Millard on 2019-11-25.
//

#include "Json.h"
#include <exception>

// *********************** Element *********************

json::Value::Value(const std::string &s) {
    type = JSON_STRING;
    data.s = new std::string(s);
}

json::Value::Value(double n) {
    type = JSON_NUMBER;
    data.n = n;
}

json::Value::Value(json::Object &&o) {
    type = JSON_OBJECT;
    data.o = new Object(o);
}

json::Value::Value(json::Array &&a) {
    type = JSON_ARRAY;
    data.a = new Array(a);
}

json::Value::Value(bool b) {
    type = JSON_BOOL;
    data.b = b;
}

json::Value::Value(json::Value &&v) {
    type = v.type;
    data = v.data;
    v.type = JSON_NULL;
}

json::Value::Value(const json::Value &v) {
    type = v.type;

    switch(type) {
        case JSON_STRING:
            data.s = new std::string(*v.data.s);
            break;

        case JSON_OBJECT:
            data.o = new Object(*v.data.o);
            break;

        case JSON_ARRAY:
            data.a = new Array(*v.data.a);
            break;

        default:
            data = v.data;
            break;
    }
}

//json::Value &json::Value::operator=(const json::Value &v) {
//    type = v.type;
//
//    switch(type) {
//        case JSON_STRING:
//            data.s = new std::string(*v.data.s);
//            break;
//
//        case JSON_OBJECT:
//            data.o = new Object(*v.data.o);
//            break;
//
//        case JSON_ARRAY:
//            data.a = new Array(*v.data.a);
//            break;
//
//        default:
//            data = v.data;
//            break;
//    }
//    return *this;
//}

json::Value::~Value() {
    switch(type) {
        case JSON_STRING:
            delete data.s;
            break;

        case JSON_OBJECT:
            delete data.o;
            break;

        case JSON_ARRAY:
            delete data.a;
            break;

        default:
            break;
    }
}

bool json::Value::isString() const {
    return type == JSON_STRING;
}

bool json::Value::isNumber() const {
    return type == JSON_NUMBER;
}

bool json::Value::isObject() const {
    return type == JSON_OBJECT;
}

bool json::Value::isArray() const {
    return type == JSON_ARRAY;
}

bool json::Value::isNull() const {
    return type == JSON_NULL;
}

bool json::Value::isBool() const {
    return type == JSON_BOOL;
}

std::string json::Value::getString() const {
    return *data.s;
}

double json::Value::getNumber() const {
    return data.n;
}

const json::Object &json::Value::getObject() const {
    return *data.o;
}

const json::Array &json::Value::getArray() const {
    return *data.a;
}

bool json::Value::getBool() const {
    return data.b;
}

// *********************** Object *********************

//const std::vector<std::string> &json::Object::getKeys() const {
//    return
//}

bool json::Object::containsKey(const std::string &key) const {
    return keyMap.find(key) == keyMap.end();
}

const json::Value &json::Object::operator[](const std::string &key) const {
    return keyMap.at(key);
}

json::Object::Object(std::map<std::string, json::Value> keyMap): keyMap(keyMap) {

}

json::Object::Object(const json::Object &o): keyMap(o.keyMap) {}

// *********************** Array *********************

int json::Array::getLength() const {
    return values.size();
}

const json::Value &json::Array::operator[](int i) const {
    return values[i];
}

json::Array::Array(std::vector<json::Value> values): values(values) {}

json::Array::Array(const json::Array &a): values(a.values) {}

// *********************** Lexing *********************

enum TokenType {
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    COMMA,
    COLON,
    STRING,
    NUMBER,
    WHITESPACE,
};

struct JsonToken {
    TokenType type;
    int start;
    int length;

    JsonToken(TokenType type, int start, int length): type(type), start(start), length(length) {}
};

std::string getTokenStringRep(TokenType t) {
    switch (t) {
        case LEFT_BRACE: return "LEFT BRACE";
        case RIGHT_BRACE: return "RIGHT_BRACE";
        case LEFT_BRACKET: return "LEFT_BRACKET";
        case RIGHT_BRACKET: return "RIGHT_BRACKET";
        case COMMA: return "COMMA";
        case COLON: return "COLON";
        case STRING: return "STRING";
        case NUMBER: return "NUMBER";
        case WHITESPACE: return "WHITESPACE";
    }
}

std::string getTokenValue(JsonToken t, const std::string &s) {
    return s.substr(t.start, t.length);
}


class JsonParseException : std::exception {
private:
    std::string desc;
public:
    JsonParseException(std::string desc, int idx) {
        this->desc = std::to_string(idx) +": "+ desc;
    }

    virtual const char* what() const throw() {
        return desc.c_str();
    }
};

JsonToken getStringToken(const std::string json, int startIdx) {
    bool isEscaped = false;
    int curIdx = startIdx + 1; // assume the first character is "
    while (curIdx < json.size())
    {
        char c = json[curIdx];
        if (isEscaped) {
            switch (c) {
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                case '"':
                case '\\':
                    isEscaped = false;
                    curIdx++;
                    break;
                default:
                    throw JsonParseException("invalid character while lexing string", curIdx);
            }
        } else {
            switch (c) {
                case '"':
                    return JsonToken(STRING, startIdx, curIdx-startIdx+1);
                case '\\':
                    isEscaped = true;
                    curIdx++;
                    break;
                default:
                    curIdx++;
            }

        }

    }

    throw JsonParseException("reached end of json while lexing string", curIdx);
}


JsonToken getNumberToken(const std::string json, int startIdx)
{
    int curIdx = startIdx + 1;

    while (curIdx < json.size()) {
        char c = json[curIdx];
        if (c == '.' || c == 'e' || (c >= '0' && c <= '9')) {
            curIdx++;
        } else {
            return JsonToken(NUMBER, startIdx, curIdx-startIdx);
        }
    }
    throw JsonParseException("reached end of string while lexing number", curIdx);
}

std::vector<JsonToken> tokenize(const std::string &json) {
    std::vector<JsonToken> ret;
    int idx = 0;
    while (idx < json.size())
    {
        char c = json[idx];
        switch (c) {
            case '{':
                ret.emplace_back(LEFT_BRACE, idx, 1);
                idx++;
                break;

            case '}':
                ret.emplace_back(RIGHT_BRACE, idx, 1);
                idx++;
                break;

            case '[':
                ret.emplace_back(LEFT_BRACKET, idx, 1);
                idx++;
                break;

            case ']':
                ret.emplace_back(RIGHT_BRACKET, idx, 1);
                idx++;
                break;

            case ',':
                ret.emplace_back(COMMA, idx, 1);
                idx++;
                break;

            case ':':
                ret.emplace_back(COLON, idx, 1);
                idx++;
                break;

            case '"':
                {
                    JsonToken tok = getStringToken(json, idx);
                    ret.push_back(tok);
                    idx += tok.length;
                }
                break;

            case ' ':
            case '\t':
            case '\n':
            case '\r':
            case '\f':
                idx++;
                break;

            default:
                if ((c >= '0' && c <= '9') || c == '-') {
                    JsonToken tok = getNumberToken(json, idx);
                    ret.push_back(tok);
                    idx += tok.length;
                } else {
                    throw JsonParseException("invalid character encountered during lexing", idx);
                }
        }
    }
    return ret;
}

#include <iostream>

void json::printJsonTokens(const std::string &json) {
    auto tokens = tokenize(json);
    for (JsonToken &tok : tokens) {
        std::cout << getTokenStringRep(tok.type) << ":" << getTokenValue(tok, json) << "\n";
    }
}

// *********************** Parsing *********************



std::pair<json::Object, int> parseObject(const std::vector<JsonToken> tokens, const std::string json, const int tokIdx);
std::pair<json::Array, int> parseArray(const std::vector<JsonToken> tokens, const std::string json, const int tokIdx);

std::pair<json::Value, int> parseValue(const std::vector<JsonToken> tokens, const std::string json, const int tokIdx) {
    switch (tokens[tokIdx].type) {
        case LEFT_BRACE:
            return parseObject(tokens, json, tokIdx);

        case LEFT_BRACKET:
            return parseArray(tokens, json, tokIdx);

        case STRING:
        {
            std::string tokVal = getTokenValue(tokens[tokIdx], json);
            return std::make_pair(tokVal,tokIdx+1);
        }

        case NUMBER:
        {
            std::string tokVal = getTokenValue(tokens[tokIdx], json);
            return std::make_pair(std::stod(tokVal),tokIdx+1);
        }

        default:
            throw JsonParseException("invalid token during value parsing", tokIdx);

    }
}

std::pair<std::pair<std::string,json::Value>,int> parseKVPair(const std::vector<JsonToken> tokens, const std::string json, const int tokIdx) {
    int curIdx = tokIdx;

    std::string key;
    if (tokens[curIdx].type != STRING) {
        throw JsonParseException("invalid token during key value parsing", curIdx);
    }
    key = getTokenValue(tokens[curIdx], json);
    curIdx++;

    if (tokens[curIdx].type != COLON) {
        throw JsonParseException("invalid token during key value parsing", curIdx);
    }
    curIdx++;

    auto x = parseValue(tokens, json, curIdx);
    curIdx = x.second;

    return std::make_pair(std::make_pair(key, x.first), curIdx);
}

std::pair<json::Object, int> parseObject(const std::vector<JsonToken> tokens, const std::string json, const int tokIdx) {
    int curIdx = tokIdx;
    if (tokens[curIdx].type != LEFT_BRACE) {
        throw JsonParseException("invalid token during object parsing", curIdx);
    }
    curIdx++;

    std::map<std::string,json::Value> keyMap;

    bool firstToken = true;
    bool lastTokenWasComma = false;

    while (curIdx < tokens.size()) {
        if (tokens[curIdx].type == RIGHT_BRACE) {
            if (lastTokenWasComma) {
                throw JsonParseException("last token was comma during object parsing", curIdx);
            }
            return std::make_pair(json::Object(keyMap),curIdx+1);

        } else if (tokens[curIdx].type == COMMA) {
            if (lastTokenWasComma) {
                throw JsonParseException("double comma encountered during object parsing", curIdx);
            }
            if (firstToken) {
                throw JsonParseException("first token was comma during object parsing", curIdx);
            }

            lastTokenWasComma = true;
            curIdx++;

        } else {
            auto x = parseKVPair(tokens, json, curIdx);
            auto &kvPair = x.first;
            keyMap.insert(std::make_pair(kvPair.first, kvPair.second));
            curIdx = x.second;
            firstToken = false;
            lastTokenWasComma = false;
        }
    }
    throw JsonParseException("ran out of tokens during object parsing", curIdx);
}

std::pair<json::Array, int> parseArray(const std::vector<JsonToken> tokens, const std::string json, const int tokIdx) {
    int curIdx = tokIdx;
    if (tokens[curIdx].type != LEFT_BRACKET) {
        throw JsonParseException("invalid token during object parsing", curIdx);
    }
    curIdx++;

    std::vector<json::Value> values;

    bool firstToken = true;
    bool lastTokenWasComma = false;

    while (curIdx < tokens.size()) {
        if (tokens[curIdx].type == RIGHT_BRACKET) {
            if (lastTokenWasComma) {
                throw JsonParseException("last token was comma during array parsing", curIdx);
            }
            return std::make_pair(json::Array(values),curIdx+1);

        } else if (tokens[curIdx].type == COMMA) {
            if (lastTokenWasComma) {
                throw JsonParseException("double comma encountered during array parsing", curIdx);
            }
            if (firstToken) {
                throw JsonParseException("first token was comma during object parsing", curIdx);
            }

            lastTokenWasComma = true;
            curIdx++;

        } else {
            auto x = parseValue(tokens, json, curIdx);
            values.emplace_back(x.first);
            curIdx = x.second;

            firstToken = false;
            lastTokenWasComma = false;

        }
    }
    throw JsonParseException("ran out of tokens during array parsing", curIdx);
}



json::Value parse(const std::string &json) {
    std::vector<JsonToken> tokens = tokenize(json);
    return parseValue(tokens, json, 0).first;
}


// *********************** Document *********************

json::Document::Document(const std::string &json): value(parse(json)) {}

const json::Value &json::Document::getValue() const {
    return value;
}

