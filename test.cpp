/*
 * This software is licensed under the STAHP-PLAYING-WITH-MAH-TOYZ public license.
 *
 * It is additionally public domain.
 */

#include "dson.hpp"

#include <iostream>

int main() {
    DsonValue* v = parseDsonV2("such \"foo\" is 42, \"bar\" is 10 wow");
    if(v->getEntryType() == ERROR) {
        std::cout << "FAIL: " << static_cast<DsonError*>(v)->what() << std::endl;
        return 0;
    }
    std::wcout << static_cast<DsonNumber*>(static_cast<DsonObject*>(v)->val[L"bar"])->val << std::endl;
}