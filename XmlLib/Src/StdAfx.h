// stdafx.h

#pragma once
#pragma warning(disable: 4786) // identifier was truncated to '255' characters in the debug information

#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0400

#include <windows.h>

#define USHORTPTRPTR(x) ((unsigned short **)(void**)x)
#define USHORTPTR(x) ((unsigned short *)(void*)x)
#define CWCHARTPTR(x) ((const wchar_t *)(void*)x)
