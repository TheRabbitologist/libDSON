/*
 * This software is licensed under the STAHP-PLAYING-WITH-MAH-TOYZ public license.
 *
 * It is additionally public domain.
 */

#include "dson.hpp"

#include <iostream>

int main() {
    DsonObject* obj = parseDsonV2Object("such \"bar\" is so 14.2 also yes many , \"foo\" is such \"marco\" is \"polo\" wow wow");
    obj->serialize(std::cout);
    std::cout << std::endl;
}