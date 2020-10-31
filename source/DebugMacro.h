#pragma once

// for debug output
#define DEBUG false
#ifdef DEBUG
#  define DEBUG_MODE(x) x
#else
#  define DEBUG_MODE(x) 
#  pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif