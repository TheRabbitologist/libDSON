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

namespace dson {
    
    enum DsonEntryType {
        STRING = 1, NUMBER = 2, BOOLEAN = 3, ARRAY = 4, OBJECT = 5,
        EMPTY = 0, ERROR = -1,
        END_OBJ = 25, END_ARR = 24, DELIM_OBJ = 15, DELIM_ARR = 14, IS = 35
    };

    struct DsonValue {
        DsonValue() : type(EMPTY) {};
        inline DsonEntryType getEntryType() {return type;}
        inline bool isValue() {
            return type == STRING ||
                    type == NUMBER ||
                    type == BOOLEAN ||
                    type == ARRAY ||
                    type == OBJECT;
        }
        inline bool isError() {return type == ERROR;}
        virtual void serialize(std::ostream& out) {}
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
        DsonString(std::wstring str) : DsonValue(STRING), val(str) {}
        DsonString() : DsonValue(STRING) {}
        void serialize(std::ostream& out);
        std::wstring val;
    };

    struct DsonNumber : public DsonValue {
        DsonNumber(double value) : DsonValue(NUMBER), val(value) {}
        DsonNumber() : DsonValue(NUMBER) {}
        void serialize(std::ostream& out);
        double val;
    };

    struct DsonArray : public DsonValue {
        DsonArray(const std::vector<DsonValue*>& arr) : DsonValue(ARRAY), val(arr) {}
        DsonArray() : DsonValue(ARRAY) {}
        void serialize(std::ostream& out);
        std::vector<DsonValue*> val;
    };

    struct DsonObject : public DsonValue {
        DsonObject(std::map<std::wstring, DsonValue*> obj) : DsonValue(OBJECT), val(obj) {}
        DsonObject() : DsonValue(OBJECT) {}
        void serialize(std::ostream& out);
        std::map<std::wstring, DsonValue*> val;
    };

    struct DsonBoolean : public DsonValue {
        DsonBoolean(bool boolean) : DsonValue(BOOLEAN), val(boolean) {}
        DsonBoolean() : DsonValue(BOOLEAN) {}
        void serialize(std::ostream& out);
        bool val;
    };

    DsonValue* parseDsonV2Value(std::istream& in);
    DsonValue* parseDsonV2Value(std::istream&& in);
    inline DsonValue* parseDsonV2Value(const std::string& in) {
        return parseDsonV2Value(std::stringstream(in));
    }

    DsonObject* parseDsonV2Object(std::istream& in);
    DsonObject* parseDsonV2Object(std::istream&& in);
    inline DsonObject* parseDsonV2Object(const std::string& in) {
        return parseDsonV2Object(std::stringstream(in));
    }

    DsonArray* parseDsonV2Array(std::istream& in);
    DsonArray* parseDsonV2Array(std::istream&& in);
    inline DsonArray* parseDsonV2Array(const std::string& in) {
        return parseDsonV2Array(std::stringstream(in));
    } 
}

#endif	/* DSON_HPP */

