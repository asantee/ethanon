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

#ifndef ETH_PROJECT_MANAGER_H_
#define ETH_PROJECT_MANAGER_H_

#include "EditorBase.h"

#include <stdio.h>
#include <string>
#include <iostream>

#define _ETH_PROJECT_MANAGER_WINDOW_TITLE GS_L("Ethanon Editor")

class ProjectManager : public EditorBase
{
	struct ETH_FILE_TO_COPY
	{
		std::string file;
		bool overwrite;
	};

public:
	ProjectManager(ETHResourceProviderPtr provider);
	void LoadEditor();
	std::string DoEditor(SpritePtr pNextAppButton);
	void Clear()
	{
		// dummy...
	}
	void StopAllSoundFXs()
	{
		// dummy... no SFX to stop
	}
	bool ProjectManagerRequested()
	{
		// dummy
		return true;
	}

	void RequestProjectOpen(const std::string& path);

private:
	bool Save();
	bool Open();
	bool SaveAs();
	void SaveLastDir(const char *szLastDir);
	std::string ReadLastDir();
	void PrepareProjectDir();
	void CreateFileMenu();

	void ProccessFileOpenRequests();

	std::string m_requestedProjectFileNameToOpen;
};

#endif