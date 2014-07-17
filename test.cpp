/*
 * This software is licensed under the STAHP-PLAYING-WITH-MAH-TOYZ public license.
 *
 * It is additionally public domain.
 */

#include "dson.hpp"

#include <iostream>

int main() {
	std::vector<bool> b = {true, false, false, true};
	std::vector<int> t = {1, 2, 4, 8, 16, 32};
	std::vector<float> f = {6,6,6,6,6.5};
	std::vector<std::string> s = {"inter","course"};
	std::vector<std::wstring> w = {L"I see you shiver with antici",L"pation."};
	
	//dson::DsonArray ab(b.begin(),b.end());
	dson::DsonArray tb(t.begin(),t.end());
	dson::DsonArray fb(f.begin(),f.end());
	dson::DsonArray sb(s.begin(),s.end());
	dson::DsonArray wb(w.begin(),w.end());
	
    std::cout << std::endl;
}