/*Copyright (c) 2014 TheRabbitologist

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <string>
#include <sstream>
#include <vector>
#include <map>

#ifndef DSON_HPP
#define	DSON_HPP

enum DsonEntryType {
    STRING, NUMBER, ARRAY, OBJECT, BOOLEAN, EMPTY, ERROR
};

enum ImplementPreference {
    SMALL, FAST
};

struct DsonValue {
    DsonValue() : type(EMPTY) {};
    inline DsonEntryType getEntryType() {return type;}
protected:
    DsonValue(DsonEntryType t) {this->type = t;}
    DsonEntryType type;
};

struct DsonError : public DsonValue {
    explicit DsonError(std::string what) : DsonValue(ERROR), desc(what) {}
    const char* what() {return desc.c_str();}
private:
    const std::string desc;
};

struct DsonString : public DsonValue {
    DsonString() : DsonValue(STRING) {}
    std::string val;
};

struct DsonNumber : public DsonValue {
    DsonNumber() : DsonValue(NUMBER) {}
    double val;
};

struct DsonArray : public DsonValue {
    DsonArray() : DsonValue(ARRAY) {}
    std::vector<DsonValue> val;
};

struct DsonObject : public DsonValue {
    DsonObject() : DsonValue(OBJECT) {}
    std::map<std::string,DsonValue> val;
};

struct DsonBoolean : public DsonValue {
    DsonBoolean() : DsonValue(BOOLEAN) {}
    bool val;
};

DsonValue* parseDson(std::istream& in);
DsonValue* parseDson(std::istream&& in);

inline DsonValue* parseDson(const std::string& in) {
    return parseDson(std::stringstream(in));
}

#endif	/* DSON_HPP */

