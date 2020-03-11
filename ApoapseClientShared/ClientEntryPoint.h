// ----------------------------------------------------------------------------
// Copyright (C) 2020 Apoapse
// Copyright (C) 2020 Guillaume Puyal
//
// Distributed under the Apoapse Pro Client Software License. Non-commercial use only.
// See accompanying file LICENSE.md
//
// For more information visit https://github.com/apoapse/
// And https://apoapse.space/
// ----------------------------------------------------------------------------

#pragma once
#include <vector>
#include "TypeDefs.hpp"
#include <string>
#include <memory>
#include "DllExportSymbolApi.hpp"
#include "ISignalSender.hpp"

namespace ApoapseClientEntry
{
DLL_EXPOSE_SYMBOL int ClientMain(const std::vector<std::string>& launchArgs);
DLL_EXPOSE_SYMBOL void Shutdown();

DLL_EXPOSE_SYMBOL std::vector<byte> ReadFile(const std::string& filename, const std::string& fileExtension);
DLL_EXPOSE_SYMBOL void RegisterSignalSender(ISignalSender* signalSender);
DLL_EXPOSE_SYMBOL std::string OnReceivedSignal(const std::string& name, const std::string& data);
DLL_EXPOSE_SYMBOL void SetLastFilesDrop(const std::vector<std::string>& files);
}