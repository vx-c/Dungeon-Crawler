#pragma once

// for debug output
#define DEBUG false
#ifdef DEBUG
#  define DEBUG_MODE(x) x
#else
#  define DEBUG_MODE(x) 
#endif
#include <iostream>