#pragma once

#include <string>
#include <wchar.h>

#include <codecvt>
#include <locale>

// convert a string to a wstring
std::wstring stringToWString(std::string s);

// convert a wstring to a string
std::string wStringToString(std::wstring s);
