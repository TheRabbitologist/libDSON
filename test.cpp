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
    std::wcout << static_cast<DsonString*>(static_cast<DsonObject*>(static_cast<DsonArray*>(v)->val[1])->val[L"foo"])->val << std::endl;
}