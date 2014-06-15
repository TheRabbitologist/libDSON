/*
 * This software is licensed under the STAHP-PLAYING-WITH-MAH-TOYZ public license.
 *
 * It is additionally public domain.
 */

#include "dson.hpp"

#include <iostream>

int main() {
    DsonValue* v = parseDsonV2Value("so 42 also such \"foo\" is \"bar\" wow and 13 many");
    if(v->getEntryType() == ERROR) {
        std::cout << "FAIL: " << static_cast<DsonError*>(v)->what() << std::endl;
        return 0;
    }
    DsonArray arr;
    arr.val.push_back(new DsonString(L"I hate coffee"));
    arr.val.push_back(new DsonString(L"I love tea"));
    arr.val.push_back(new DsonString(L"Intercourse OJ"));
    arr.serialize(std::cout);
    std::cout << std::endl;
}