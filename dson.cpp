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

struct DsonPrivDelim : public DsonValue {
    explicit DsonPrivDelim() : DsonValue(DELIM) {}
};

struct DsonPrivEnd : public DsonValue {
    explicit DsonPrivEnd() : DsonValue(END) {}
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
    std::string str = read(in);
    return new DsonError("NYI");
}

static DsonArray* makeArray(std::istream& in) {
    DsonArray* arr = new DsonArray();
    DsonValue* valu = parseValue(in);
    while(valu->getEntryType() != END && valu->getEntryType() != ERROR) {
        arr->val.push_back(valu);
        valu = parseValue(in);
    }
    return arr;
}

static DsonObject* makeObject(std::istream& in) {
    std::string str;
    for(str = read(in); str != "wow"; str = read(in)) {
        
    }
    return new DsonObject();
}

DsonValue* parseDson(std::istream& in) {
    return parseValue(in);
}

DsonValue* parseDson(std::istream&& in) {
    return parseValue(in);
}