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

#include "stdafx.h"
#include "Common.h"
#include "ImageUtils.h"
#include <opencv2/opencv.hpp>
#include <filesystem>

std::vector<byte> ImageUtils::ReadAndResizeImage(const std::string& filePath, int newWidth, int newHeight, bool preserveRatio, const std::string& forceEncoding/* = std::string()*/)
{
	std::vector<byte> output;
	cv::Mat outputImg;
	const cv::Mat original = cv::imread(filePath);

	const int originalWidth = original.size().width;
	const int originalHeight = original.size().height;
	int withToResize = newWidth;
	int heightToResize = newHeight;

	if (preserveRatio)
	{
		if (originalWidth > original.size().height)
		{
			withToResize = newWidth;
			heightToResize = static_cast<int>(newHeight / ((float)originalWidth / (float)originalHeight));
		}
		else
		{
			withToResize = static_cast<int>(newWidth / ((float)originalHeight / (float)originalWidth));
			heightToResize = newHeight;
		}
	}

	cv::resize(original, outputImg, cv::Size(withToResize, heightToResize), 0, 0, cv::INTER_CUBIC);

	std::string fileExtension;
	if (!forceEncoding.empty())
		fileExtension = "." + forceEncoding;
	else
		fileExtension = std::filesystem::path(filePath).extension().string();
	
	const size_t sizeInBytes = outputImg.step[0] * outputImg.rows;

	output.resize(sizeInBytes);
	cv::imencode(fileExtension, outputImg, output);
	
	return output;
}
