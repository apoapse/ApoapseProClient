#include "stdafx.h"
#include "WebResourcesManager.h"
#include "Common.h"
#include <fstream>

std::vector<byte> WebResourcesManager::ReadFile(const std::string& filename, const std::string& fileExtension)
{
	std::vector<byte> output;

	try
	{
		std::ifstream inputStream(filename, std::ios::binary | std::ios::ate);

		if (inputStream.is_open())
		{
			const size_t size = static_cast<size_t>(inputStream.tellg());
			inputStream.seekg(0);

			char* raw = new char[size];
			inputStream.read(raw, size);

			output.resize(size);
			std::copy(raw, raw + size, output.begin());
			delete[] raw;

			ASSERT(output.size() == size);
			LOG << "Loaded web resource " << filename << " length: " << size << " bytes";

			inputStream.close();
		}
		else
			throw std::exception("inputStream is not open");
	}
	catch (const std::exception&)
	{
		LOG << "Unable to load the file: " << filename << '\n' << LogSeverity::warning;
	}

	return output;
}
