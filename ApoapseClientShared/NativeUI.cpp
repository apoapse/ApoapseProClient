#include "stdafx.h"
#include "Common.h"
#include "NativeUI.h"
#include <commdlg.h>
#include "StringExtensions.h"
#include <sstream>

std::string NativeUI::OpenFileDialog(const std::vector<std::string>& extensionsSupported, const std::string& extensionsGroup)
{
	OPENFILENAME ofn ;
	char szFile[100];
	
	std::stringstream ss;
	ss << extensionsGroup << '\0';
	ss << StringExtensions::join(extensionsSupported, ";", true);
	ss << '\0';
	const std::string filter = ss.str();
	
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL  ;
	ofn.lpstrFile = szFile ;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter.c_str();
	//ofn.lpstrFilter = "Images\0*.jpg;*.jpeg;*.png;*.gif\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
	GetOpenFileName(&ofn);

	return std::string(szFile);
}
