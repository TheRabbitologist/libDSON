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

#include "dson.hpp"
#include <cctype>
#include <string>

struct DsonFormat : public DsonValue {
    explicit DsonFormat(DsonValue val) : DsonValue(val) {}
};

static std::string read(std::istream& in) {
    std::string str;
    bool ws = true;
    char c;
    while(ws) {
        c = in.get();
        if(!std::isspace(c))
            ws = false;
    }
    while(!ws) {
        str.push_back(c);
        c = in.get();
        if(std::isspace(c))
            ws = true;
    }
    return str;
}

static DsonValue* parseValue(std::istream& in) {
    return new DsonError("NYI");
}

static DsonValue* makeArray(std::istream& in) {
    DsonArray* arr = new DsonArray();
    DsonValue* valu = parseValue(in);
    while(valu->getEntryType() != END) {
        if(valu->getEntryType() == ERROR) {
            delete arr;
            return valu;
        }
        if(valu->getEntryType() != DELIM)
            arr->val.push_back(valu);
        else
            delete valu;
        valu = parseValue(in);
    }
    delete valu;
    return arr;
}

static DsonValue* makeObject(std::istream& in) {
    DsonObject* obj = new DsonObject();
    DsonValue* valu = parseValue(in);
    std::string temp;
    while(valu->getEntryType() != END) {
        if(valu->getEntryType() == ERROR) {
            delete obj;
            return valu;
        }
        if(temp.empty()) {
            if(valu->getEntryType() != STRING) {
                delete valu;
                return new DsonError("Expected string as identifier for a DSON object");
            }
            temp.assign(static_cast<DsonString*>(valu)->val);
            delete valu;
        }
        else {
            if(valu->getEntryType() != DELIM) {
                obj->val[temp] = valu;
                temp.clear();
            }
            else
                delete valu;
        }
        valu = parseValue(in);
    }
    return obj;
}

DsonValue* parseDson(std::istream& in) {
    return parseValue(in);
}

DsonValue* parseDson(std::istream&& in) {
    return parseValue(in);
}