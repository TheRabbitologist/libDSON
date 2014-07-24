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
#include <memory>
#include <type_traits>

#ifndef DSON_HPP
#define	DSON_HPP

namespace dson {

enum DsonEntryType {
	STRING = 1, NUMBER = 2, BOOLEAN = 3, ARRAY = 4, OBJECT = 5,
	EMPTY = 0, ERROR = -1,
	END_OBJ = 25, END_ARR = 24, DELIM_OBJ = 15, DELIM_ARR = 14, IS = 35
};

class DsonValue {
public:
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

class DsonError : public DsonValue {
	const std::string desc;
public:
	explicit DsonError(std::string what) : DsonValue(ERROR), desc(what) {}
	const char* what() {return desc.c_str();}
	void serialize(std::ostream& out) {throw std::logic_error("Attempted to serialize a DsonError object");}
};

class DsonString : public DsonValue {
	std::wstring val;
public:
	decltype(val)& data() {return val;}
	DsonString(const std::wstring& str) : DsonValue(STRING), val(str) {}
	DsonString(const std::string& str) : DsonValue(STRING), val(str.begin(), str.end()) {}
	DsonString() : DsonValue(STRING) {}
	void serialize(std::ostream& out);
	inline void set(const std::string& str) {val = std::wstring(str.begin(), str.end());}
	inline void set(const std::wstring& value) {val = value;}
	inline const std::wstring& get() {return val;}
	inline void append(std::wstring::value_type ch) {val.push_back(ch);}
	inline void append(const std::wstring& str) {val+=str;}
	inline void append(const std::string& str) {
		std::wstring wstr(str.begin(),str.end());
		val+=wstr;
	}
	inline size_t size() {return val.size();}
};

struct DsonNumber : public DsonValue {
	double val;
public:
	DsonNumber(double value) : DsonValue(NUMBER), val(value) {}
	DsonNumber() : DsonValue(NUMBER) {}
	void serialize(std::ostream& out);
	inline void set(double value = 0.0) {val = value;}
	inline double get() {return val;}
};

class DsonBoolean : public DsonValue {
	bool val;
public:
	DsonBoolean(bool boolean) : DsonValue(BOOLEAN), val(boolean) {}
	DsonBoolean() : DsonValue(BOOLEAN) {}
	void serialize(std::ostream& out);
	inline void flip() {val = !val;}
	inline void set(bool value = true) {val = value;}
	inline bool get() {return val;}
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
template <typename T,
		  typename std::enable_if<std::is_same<T,DsonValue>::value>::type* = nullptr>
DsonValue* makeValue(T val) {
	return &val;
}
template <typename T,
		  typename std::enable_if<std::is_same<T,DsonValue*>::value>::type* = nullptr>
DsonValue* makeValue(T val) {
	return val;
}

class DsonArray : public DsonValue {
	std::vector<std::unique_ptr<DsonValue>> val;
public:
	inline decltype(val)& data() {return val;}
	template <typename Iterator>
	DsonArray(Iterator begin, Iterator end) : DsonValue(ARRAY) {
		while (begin != end) {
			val.emplace_back(std::unique_ptr<DsonValue>(makeValue(*begin)));
			++begin;
		}
	}
	DsonArray() : DsonValue(ARRAY) {}
	void serialize(std::ostream& out);
	DsonValue& operator[](size_t indx) {return *val[indx];}
	inline void push_back(DsonValue* value) {
		val.emplace_back(std::unique_ptr<DsonValue>(value));
	}
	inline void set(size_t indx, DsonValue* value) {
		val.emplace(val.begin()+indx,std::unique_ptr<DsonValue>(value));
	}
	inline const DsonValue& get(size_t indx) {
		return *val.at(indx);
	}
	inline size_t size() {return val.size();}
};

class DsonObject : public DsonValue {
	std::map<std::wstring, std::unique_ptr<DsonValue>> val;
public:
	inline decltype(val)& data() {return val;}
	DsonObject() : DsonValue(OBJECT) {}
	void serialize(std::ostream& out);
	DsonValue& operator[](const std::wstring& key) {return *val[key];}
	DsonValue& operator[](const std::string& key) {return *val[std::wstring(key.begin(),key.end())];}
	inline void set(const std::wstring& key, DsonValue* value) {
		val.emplace(std::make_pair(key, std::unique_ptr<DsonValue>(value)));
	}
	inline void set(const std::string& key, DsonValue* value) {
		val.emplace(std::make_pair(std::wstring(key.begin(),key.end()), std::unique_ptr<DsonValue>(value)));
	}
	inline const DsonValue& get(const std::wstring& key) {
		return *val.at(key);
	}
	inline const DsonValue& get(const std::string& key) {
		return *val.at(std::wstring(key.begin(),key.end()));
	}
	inline bool has(const std::wstring& key) {
		return val.count(key);
	}
	inline bool has(const std::string& key) {
		return val.count(std::wstring(key.begin(),key.end()));
	}
	inline size_t size() {return val.size();}
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

