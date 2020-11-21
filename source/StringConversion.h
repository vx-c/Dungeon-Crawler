#pragma once

#include <string>
#include <wchar.h>

#include <codecvt>

// convert a string to a wstring
std::wstring stringToWString(std::string s)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> coverter;
	return coverter.from_bytes(s);
}

// convert a wstring to a string
std::string wStringToString(std::wstring s)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> coverter;
	return coverter.to_bytes(s);
}
