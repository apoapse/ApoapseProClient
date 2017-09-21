#pragma once
#include <vector>
#include "TypeDefs.hpp"
#include <string>
#include <memory>
#include "DllExportSymbolApi.hpp"

namespace ApoapseClient
{

DLL_EXPOSE_SYMBOL int ClientMain(const std::vector<std::string>& launchArgs);
DLL_EXPOSE_SYMBOL void Shutdown();

DLL_EXPOSE_SYMBOL std::vector<byte> ReadFile(const std::string& filename, const std::string& fileExtension);
DLL_EXPOSE_SYMBOL std::string OnReceivedSignal(const std::string& name, const std::string& data);
}