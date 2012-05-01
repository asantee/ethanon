/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#ifndef ETH_EDITORCOMMON_H_
#define ETH_EDITORCOMMON_H_

#include "../engine/ETHCommon.h"

#ifdef _MSC_VER
	#define _ETH_SAFE_strcat strcat_s
	#define _ETH_SAFE_sscanf sscanf_s
	#define _ETH_SAFE_sprintf sprintf_s
	inline void _ETH_SAFE_strcpy(char *szOut, const char *szIn)
	{
		strcpy_s(szOut, strlen(szIn)+1, szIn);
	}	
#else
	#define _ETH_SAFE_strcat strcat
	#define _ETH_SAFE_sscanf sscanf
	#define _ETH_SAFE_sprintf sprintf
	#define _ETH_SAFE_strcpy strcpy
#endif

namespace ETHGlobal {

	ETH_INLINE std::string GetPathName(const char *szSource, const bool keepLastSlash)
	{
		std::string r = szSource;
		const unsigned int nLen = strlen(szSource);
		if (nLen <= 1)
			return "";
		unsigned int t;
		for (t=nLen-1; t>0; t--)
		{
			if (szSource[t] == '\\' || szSource[t] == '/')
			{
				t++;
				break;
			}
		}
		if (keepLastSlash)
		{
			r.resize(t);
		}
		else
		{
			r.resize(t-1);
		}
		return r;
	}

	ETH_INLINE std::string GetFileName(const std::string &source)
	{
		const unsigned int nLen = source.length();
		std::string r = source;
		unsigned int t;
		for (t=nLen-1; t>0; t--)
		{
			if (r[t] == '\\' || r[t] == '/')
			{
				r = r.substr(t+1);
				break;
			}
		}
		return r;
	}

	ETH_INLINE std::string GetDirectoryFromPath(const char *szCurrentPath)
	{
		std::string path = szCurrentPath;
		const int nLen=path.size();
		for (int t=nLen; t>=0; t--)
		{
			if (path[t] == '\\')
			{			
				path.resize(t);
				break;
			}
		}
		return path;
	}

} // namespace ETHGlobal

#endif