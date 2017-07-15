// XmlLibExports.h
//
#pragma once

#ifndef XMLLIB_EXPORTS
 #ifdef _DEBUG
  #pragma comment(lib, "XmlLibD.lib")
 #else
  #pragma comment(lib, "XmlLib.lib")
 #endif
#endif
