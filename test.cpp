/*
 * This software is licensed under the STAHP-PLAYING-WITH-MAH-TOYZ public license.
 *
 * It is additionally public domain.
 */

#include "dson.hpp"

#include <iostream>

int main() {
    DsonValue* v = parseDson("42very3");
    if(v->getEntryType() == ERROR) {
        std::cout << "FAIL: " << static_cast<DsonError*>(v)->what() << std::endl;
        return 0;
    }
    std::cout << static_cast<DsonNumber*>(v)->val << std::endl;
}