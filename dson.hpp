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
#include <type_traits>

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
		void serialize(std::ostream& out) {throw std::logic_error("Attempted to serialize a DsonError object");}
    private:
        const std::string desc;
    };

    struct DsonString : public DsonValue {
        DsonString(const std::wstring& str) : DsonValue(STRING), val(str) {}
        DsonString(const std::string& str) : DsonValue(STRING), val(str.begin(), str.end()) {}
        DsonString() : DsonValue(STRING) {}
        void serialize(std::ostream& out);
		inline void set(const std::string& str) {val = std::wstring(str.begin(), str.end());}
        std::wstring val;
    };

    struct DsonNumber : public DsonValue {
        DsonNumber(double value) : DsonValue(NUMBER), val(value) {}
        DsonNumber() : DsonValue(NUMBER) {}
        void serialize(std::ostream& out);
        double val;
    };
	
    struct DsonBoolean : public DsonValue {
        DsonBoolean(bool boolean) : DsonValue(BOOLEAN), val(boolean) {}
        DsonBoolean() : DsonValue(BOOLEAN) {}
        void serialize(std::ostream& out);
        bool val;
    };
	 
	template <typename T,
		typename std::enable_if<(std::is_integral<T>::value && !std::is_same<T,bool>::value) || std::is_floating_point<T>::value>::type* = nullptr>
	DsonValue* makeValue(T val) {
		return new DsonNumber(val);
	}
	template <typename T,
		typename std::enable_if<std::is_same<T,std::string>::value || std::is_same<T,std::wstring>::value>::type* = nullptr>
	DsonValue* makeValue(T val) {
		return new DsonString(val);
	}
	template <typename T,
		typename std::enable_if<std::is_same<T,bool>::value || (std::is_convertible<T,bool>::value && std::is_compound<T>::value)>::type* = nullptr>
	DsonValue* makeValue(T val) {
		return new DsonBoolean(val);
	}

    struct DsonArray : public DsonValue {
		template <typename Iterator>
        DsonArray(Iterator begin, Iterator end) : DsonValue(ARRAY) {
			while (begin != end) {
				val.push_back(makeValue(*begin));
				++begin;
			}
		}
        DsonArray(const std::vector<DsonValue*>& arr) : DsonValue(ARRAY), val(arr) {}
        DsonArray() : DsonValue(ARRAY) {}
        void serialize(std::ostream& out);
        std::vector<DsonValue*> val;
    };

    struct DsonObject : public DsonValue {
        DsonObject(const std::map<std::wstring, DsonValue*>& obj) : DsonValue(OBJECT), val(obj) {}
        DsonObject() : DsonValue(OBJECT) {}
        void serialize(std::ostream& out);
        std::map<std::wstring, DsonValue*> val;
    };

    DsonValue* parseDsonV2Value(std::istream& in);
    DsonValue* parseDsonV2Value(std::istream&& in);
    inline DsonValue* parseDsonV2Value(const std::string& in) {
        return parseDsonV2Value(std::istringstream(in));
    }

    DsonObject* parseDsonV2Object(std::istream& in);
    DsonObject* parseDsonV2Object(std::istream&& in);
    inline DsonObject* parseDsonV2Object(const std::string& in) {
        return parseDsonV2Object(std::istringstream(in));
    }

    DsonArray* parseDsonV2Array(std::istream& in);
    DsonArray* parseDsonV2Array(std::istream&& in);
    inline DsonArray* parseDsonV2Array(const std::string& in) {
        return parseDsonV2Array(std::istringstream(in));
    } 
}

#endif	/* DSON_HPP */

