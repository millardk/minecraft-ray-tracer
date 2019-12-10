//
// Created by Keegan Millard on 2019-11-25.
//

#ifndef PATH_TRACER_JSON_H
#define PATH_TRACER_JSON_H

#include <string>
#include <vector>
#include <map>

namespace json {

class Object;
class Array;

union Data {
    std::string *s;
    Object *o;
    Array *a;
    double n;
    bool b;
};

class Value {
private:
    enum ValueType {
        JSON_STRING,JSON_NUMBER,JSON_OBJECT,JSON_ARRAY,JSON_BOOL,JSON_NULL
    } type;

    Data data;

public:
    Value(): type(JSON_NULL) {}
    Value(const std::string &s);
    Value(double n);
    Value(Object &&o);
    Value(Array &&a);
    Value(bool b);
    Value(Value &&v);
    Value(const Value &v);
//    Value& operator=(const Value &v);
    ~Value();

    bool isString() const;
    bool isNumber() const;
    bool isObject() const;
    bool isArray() const;
    bool isNull() const;
    bool isBool() const;

    std::string getString() const;
    double getNumber() const;
    const Object& getObject() const;
    const Array& getArray() const;
    bool getBool() const;
};

class Object {
public:
    std::map<std::string,Value> keyMap;
    Object(const Object &o);
    explicit Object(std::map<std::string,Value> keyMap);

    bool containsKey(const std::string &key) const;
    const Value& operator[] (const std::string &key) const;
};

class Array {
public:
    std::vector<Value> values;
    Array(const Array &a);
    explicit Array(std::vector<Value> values);

    int getLength() const;
    const Value& operator[] (int i) const;

};

class Document {
private:
    Value value;
public:
    Document(const std::string &json);
    const Value& getValue() const;
};

void printJsonTokens(const std::string &json);

}

#endif //PATH_TRACER_JSON_H
