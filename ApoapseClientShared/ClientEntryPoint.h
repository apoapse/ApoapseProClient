#pragma once
#include <vector>

#ifdef DYN_LIBRARY  
#define DLL_EXPORT __declspec(dllexport)   
#else  
#define DLL_EXPORT __declspec(dllimport)   
#endif  

DLL_EXPORT int ClientMain(const std::vector<std::string>& launchArgs);
