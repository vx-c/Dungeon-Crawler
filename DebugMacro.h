#pragma once

// for debug output
#define DEBUG true
#ifdef DEBUG
#  define DEBUG_MODE(x) x
#else
#  define DEBUG_MODE(x) 
#  pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

/*
#include <json/json.h>
#include <iostream>

namespace Debug
{
	// Check if the json value has a member and output to cerr if it doesn't.
	bool checkValid(Json::Value &value, std::string member)
	{
		if (!value.isMember(member))
		{
			std::cerr << "invalid json member " << member << std::endl;
			return false;
		}
		return true;
	};
};
*/