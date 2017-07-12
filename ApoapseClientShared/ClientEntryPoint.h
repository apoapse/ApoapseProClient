#pragma once

#ifdef DYN_LIBRARY  
#define DLL_EXPORT __declspec(dllexport)   
#else  
#define DLL_EXPORT __declspec(dllimport)   
#endif  

#include <vector>

DLL_EXPORT int ClientMain(int argcount, char* argv[]);
DLL_EXPORT void Test();
