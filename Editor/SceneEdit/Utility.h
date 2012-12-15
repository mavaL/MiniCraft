#pragma once
#ifndef Utility_h__
#define Utility_h__

#include <string>

#define SAFE_DELETE(p) { if(p) { delete p; p=NULL; } }
#define SAFE_DELETE_ARRAY(a) { if(p) { delete []p; p=NULL; } }

namespace Utility
{
	std::wstring	EngineToUnicode(const char* src);
	std::wstring	EngineToUnicode(const std::string& src);
	std::string		UnicodeToEngine(const WCHAR* src);
	std::string		UnicodeToEngine(const std::wstring& src);
}



#endif // Utility_h__



