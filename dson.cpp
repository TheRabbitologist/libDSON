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
#include <cstdint>
#include <cmath>
#include <string>
#include <iostream>

const int VERY_MULT = 8;
const auto VERY_BASE = std::oct;

namespace dson {

struct DsonFormatObj : public DsonValue {
	explicit DsonFormatObj(bool b) : DsonValue(b ? END_OBJ : DELIM_OBJ) {}
	explicit DsonFormatObj() : DsonValue(IS) {}
};

struct DsonFormatArr : public DsonValue {
	explicit DsonFormatArr(bool b) : DsonValue(b ? END_ARR : DELIM_ARR) {}
};

static DsonValue* parseValue(std::istream& in);

static double octal_stod(const std::string& str) {
	size_t dot = str.find('.'), end = str.size() - 1;
	bool isdot = dot != std::string::npos;
	dot = std::min(end, dot);
	double ret = 0.0;
	for (size_t ing = dot + 1, x = 1; ing <= end; ++ing, ++x) {
		if (str[ing] >= '0' && str[ing] < '8')
			ret += static_cast<double> (str[ing] - '0') / std::pow(8, x);
		else
			return std::nan("");
	}
	for (size_t ing = dot - (isdot ? 1 : 0), x = 0; ing <= end; --ing, ++x) {
		if (ing == 0 && str[ing] == '-') {
			ret *= -1.0;
			break;
		}
		if (str[ing] >= '0' && str[ing] < '8')
			ret += (str[ing] - '0') * std::pow(8, x);
		else
			return std::nan("");
		if (ing == 0)
			break;
	};
	return ret;
}

static DsonValue* parseValueNumber(std::istream& in) {
	DsonValue* ret = nullptr;
	std::string temp;
	in >> temp;
	if (temp.length() > 1) {
		if (temp.back() == ',' || temp.back() == '.' || temp.back() == '!' || temp.back() == '?') {
			in.putback(temp.back());
			temp.pop_back();
		}
	}
	std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
	size_t pos = temp.find("very");
	if (pos != std::string::npos && pos + 4 >= temp.size())
		return new DsonError("Found \"very\" with nothing after it.");
	try {
		double val;
		if (pos != std::string::npos) {
			int exp = 1;
			std::stringstream(temp.substr(pos + 4)) >> VERY_BASE >> exp;
			val = octal_stod(temp.substr(0, pos));
			if(val == std::nan(""))
				return new DsonError("Parsed numerical value is not octal");
			val = val * std::pow(VERY_MULT, exp);
		} else {
			val = octal_stod(temp);
			if(val == std::nan(""))
				return new DsonError("Parsed numerical value is not octal");
		}
		ret = new DsonNumber();
		static_cast<DsonNumber*> (ret)->val = val;
	} catch (const std::invalid_argument& e) {
		return new DsonError(std::string("Syntax error while creating a number: ") + e.what());
	}
	return ret;
}

static DsonValue* parseValueString(std::istream& in) {
	in.ignore();
	DsonString* ret = new DsonString();
	bool escape = false;
	for (char c = in.get(); in.good(); c = in.get()) {
		if (!escape) {
			if (c == '\\') {
				escape = true;
				continue;
			} else if (c == '"')
				break;
		} else {
			escape = false;
			uint32_t t = 0;
			switch (c) {
			case '\\': break;
			case '"': break;
			case 'n': c = '\n';
				break;
			case 'b': c = '\b';
				break;
			case 't': c = '\t';
				break;
			case 'r': c = '\r';
				break;
			case 'f': c = '\f';
				break;
			case 'u': c = '\0';
				char buf[7];
				in.get(buf, 6);
				buf[6] = '\0';
				try {
					std::istringstream(buf) >> std::oct >> t;
				} catch(...) {
					delete ret;
					return new DsonError(std::string("Caught exception while parsing \\u in a string."));
				}
				if (t > std::numeric_limits<wchar_t>::max()) {
					delete ret;
					return new DsonError(std::string("Escape sequence value exceeds max length of UTF-16"));
				}
				ret->val.push_back(static_cast<wchar_t> (t));
				break;
			default:
				delete ret;
				return new DsonError(std::string("Illegal escape character: ") + c);
			}
		}
		if (c != '\0')
			ret->val.push_back(c);
	}
	return ret;
}

static DsonValue* makeArray(std::istream& in) {
	DsonArray* arr = new DsonArray();
	DsonValue* valu = parseValue(in);
	bool expectDelim = false;
	while (valu->getEntryType() != END_ARR) {
		if (valu->getEntryType() == ERROR) {
			delete arr;
			return valu;
		}
		if(expectDelim) {
			auto t = valu->getEntryType();
			delete valu;
			if(t == DELIM_ARR)
				expectDelim = false;
			else {
				delete arr;
				return new DsonError("Expected array delimiter");
			}
		} else {
			if(valu->getEntryType() != DELIM_ARR) {
				arr->val.emplace_back(std::unique_ptr<DsonValue>(valu));
				expectDelim = true;
			}
			else {
				delete valu;
				delete arr;
				return new DsonError("Unexpected array delimiter");
			}
		}
		valu = parseValue(in);
	}
	delete valu;
	return arr;
}

static DsonValue* makeObject(std::istream& in) {
	DsonObject* obj = new DsonObject();
	DsonValue* valu = parseValue(in);
	std::wstring temp;
	bool foundIs = false, expectDelim = false;
	while (valu->getEntryType() != END_OBJ) {
		if (valu->getEntryType() == ERROR) {
			delete obj;
			return valu;
		}
		if (temp.empty()) {
			if (!expectDelim && valu->getEntryType() != STRING) {
				int v = valu->getEntryType();
				delete valu;
				delete obj;
				return new DsonError("Expected string as DSON object key");
			} else if (expectDelim && valu->getEntryType() != DELIM_OBJ) {
				int v = valu->getEntryType();
				delete valu;
				delete obj;
				return new DsonError("Expected object delimiter");
			} else if (expectDelim) {
				expectDelim = false;
				delete valu;
				valu = parseValue(in);
				continue;
			}
			temp.assign(static_cast<DsonString*> (valu)->val);
			delete valu;
		} else {
			if (!foundIs && valu->getEntryType() == IS) {
				foundIs = true;
				delete valu;
			} else if (foundIs) {
				obj->val.emplace(std::make_pair(temp, std::unique_ptr<DsonValue>(valu)));
				temp.clear();
				foundIs = false;
				expectDelim = true;
			} else if (!temp.empty()) {
				delete valu;
				delete obj;
				return new DsonError("Expected 'is'");
			}
		}
		valu = parseValue(in);
	}
	delete valu;
	return obj;
}

static DsonValue* parseValue(std::istream& in) {
	char c = in.peek();
	while (std::isspace(c) && in.good()) {
		in.ignore(1);
		c = in.peek();
	}
	if (std::isdigit(c) || c == '-')
		return parseValueNumber(in);
	if (c == '"')
		return parseValueString(in);
	std::string str;
	in >> str;
	if (str.length() > 1) {
		if (str.back() == ',' || str.back() == '.' || str.back() == '!' || str.back() == '?') {
			in.putback(str.back());
			str.pop_back();
		}
	}
	if (str == "yes") {
		DsonBoolean* bl = new DsonBoolean();
		bl->val = true;
		return bl;
	}
	if (str == "no") {
		DsonBoolean* bl = new DsonBoolean();
		bl->val = false;
		return bl;
	}
	if (str == "such")
		return makeObject(in);
	if (str == "so")
		return makeArray(in);
	if (str == "," || str == "." || str == "!" || str == "?")
		return new DsonFormatObj(false);
	if (str == "and" || str == "also")
		return new DsonFormatArr(false);
	if (str == "wow")
		return new DsonFormatObj(true);
	if (str == "many")
		return new DsonFormatArr(true);
	if (str == "is")
		return new DsonFormatObj();
	if (!in.good())
		return new DsonError("Unexpected end of input");
	return new DsonError("Syntax error on '" + str + "'");
}

DsonValue* parseDsonV2Value(std::istream& in) {
	return parseValue(in);
}

DsonValue* parseDsonV2Value(std::istream&& in) {
	return parseValue(in);
}

DsonObject* parseDsonV2Object(std::istream& in) {
	std::string str;
	in >> str;
	if(str == "such") {
		DsonValue* v = makeObject(in);
		if (v->isError()) {
			std::string err = static_cast<DsonError*>(v)->what();
			delete v;
			throw std::invalid_argument(err);
		}
		return static_cast<DsonObject*> (v);
	} else
		throw std::invalid_argument("Argument is not a DSON object");
}

DsonObject* parseDsonV2Object(std::istream&& in) {
	std::string str;
	in >> str;
	if(str == "such") {
		DsonValue* v = makeObject(in);
		if (v->isError()) {
			std::string err = static_cast<DsonError*>(v)->what();
			delete v;
			throw std::invalid_argument(err);
		}
		return static_cast<DsonObject*> (v);
	} else
		throw std::invalid_argument("Argument is not a DSON object");
}

DsonArray* parseDsonV2Array(std::istream& in) {
	std::string str;
	in >> str;
	if(str == "so") {
		DsonValue* v = makeArray(in);
		if (v->isError()) {
			std::string err = static_cast<DsonError*>(v)->what();
			delete v;
			throw std::invalid_argument(err);
		}
		return static_cast<DsonArray*> (v);
	} else
		throw std::invalid_argument("Argument is not a DSON array");
}

DsonArray* parseDsonV2Array(std::istream&& in) {
	std::string str;
	in >> str;
	if(str == "so") {
		DsonValue* v = makeArray(in);
		if (v->isError()) {
			std::string err = static_cast<DsonError*>(v)->what();
			delete v;
			throw std::invalid_argument(err);
		}
		return static_cast<DsonArray*> (v);
	} else
		throw std::invalid_argument("Argument is not a DSON array");
}

void DsonBoolean::serialize(std::ostream& out) {
	out << (val ? "yes" : "no");
}

void DsonNumber::serialize(std::ostream& out) {
	decltype(out.basefield) flg = out.flags() & (out.basefield);
	double rem = val;
	if(rem <= 0.0) {
		out << '-';
		rem *= -1.0;
	}
	uint64_t whole = static_cast<uint64_t>(std::abs(val));
	out << std::oct << whole;
	rem -= whole;
	if(rem != 0.0) {
		out << '.';
		do {
			rem *= 8.0;
			unsigned int pt = static_cast<unsigned int>(rem);
			out << pt;
			rem -= pt;
		} while(rem != 0.0);
	}
	out.setf(flg);
}

void DsonString::serialize(std::ostream& out) {
	out.put('"');
	for (wchar_t t : val) {
		if (t == '\\')
			out << "\\\\";
		else if (t == '\"')
			out << "\\\"";
		else if (t == '\b')
			out << "\\b";
		else if (t == '\t')
			out << "\\t";
		else if (t == '\r')
			out << "\\r";
		else if (t == '\f')
			out << "\\f";
		else if (t == '\n')
			out << "\\n";
		else if (t > 0x7E || t < 0x20) {
			out << "\\u";
			uint16_t val = t;
			for (int i = 5; i >= 0; --i)
				out << static_cast<int> (val / std::pow(8, i)) % 8;
		} else
			out << static_cast<char> (t);

	}
	out.put('"');
}

void DsonArray::serialize(std::ostream& out) {
	bool flip = false;
	out << "so ";
	for (size_t i = 0; i < val.size(); ++i) {
		val[i]->serialize(out);
		flip = !flip;
		if (i != val.size() - 1)
			out << (flip ? " and" : " also");
		out << ' ';
	}
	out << "many";
}

void DsonObject::serialize(std::ostream& out) {
	out << "such ";
	auto e = val.end(), l = val.end();
	--l;
	for (auto i = val.begin(); i != e; ++i) {
		DsonString(i->first).serialize(out);
		out << " is ";
		i->second->serialize(out);
		if (i != l)
			out << " ,";
		out << ' ';
	}
	out << "wow";
}
}
