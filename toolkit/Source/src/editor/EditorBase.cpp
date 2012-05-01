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

#include "../engine/Platform/FileListing.h"

#include <enml/enml.h>
#include "EditorBase.h"
#include "EditorCommon.h"
#include <unicode/utf8converter.h>

#define _ENML_EDITOR_GENERAL_INFO L"data\\info.enml"

EditorBase::EditorBase(ETHResourceProviderPtr provider) :
	m_menuSize(14.0f),
	m_menuWidth(100.0f),
	m_menuScale(1.2f),
	m_background(0xFF2D3A5F),
	m_provider(provider)
{
	m_untitled = true;
	m_currentFile = _BASEEDITOR_DEFAULT_UNTITLED_FILE;
}

float EditorBase::GetMenuSize() const
{
	return m_menuSize;
}

float EditorBase::GetMenuWidth() const
{
	return m_menuWidth;
}

GS_COLOR EditorBase::GetBGColor() const
{
	return m_background;
}

void EditorBase::ReloadFiles()
{
	// dummy!
}

void EditorBase::UpdateInternalData()
{
	// dummy!
}

bool EditorBase::IsFileMenuActive() const
{
	return m_fileMenu.IsActive();
}

bool EditorBase::IsMouseOverFileMenu() const
{
	return m_fileMenu.IsMouseOver();
}

void ShadowPrint(VideoPtr video, Vector2 v2Pos, const wchar_t *text, const wchar_t *font, GS_DWORD color)
{
	video->DrawBitmapText(v2Pos, text, font, color);
}
bool EditorBase::DrawTab(VideoPtr video, InputPtr input, const Vector2 &v2Pos, const float width, const wstring &text, GS_COLOR color)
{
	video->SetAlphaMode(GSAM_PIXEL);
	video->SetVertexShader(ShaderPtr());
	video->SetPixelShader(ShaderPtr());
	video->SetZBuffer(false);
	video->SetZWrite(false);

	const Vector2 v2Cam = video->GetCameraPos();
	video->SetCameraPos(Vector2(0,0));
	bool mouseOver = false;
	if (mouseOver = ETHGlobal::PointInRect(input->GetCursorPositionF(video), v2Pos+Vector2(width/2, m_menuSize), Vector2(width, m_menuSize)))
	{
		if (color.a < 200)
			color.a = 200;
	}

	const Vector2 v2CurveSize = m_curve->GetBitmapSizeF();
	const float rectWidth = width-v2CurveSize.x*2;
	m_curve->FlipX();
	m_curve->Draw(v2Pos, color);

	const Vector2 v2RectPos = v2Pos+Vector2(v2CurveSize.x, 0);
	video->DrawRectangle(v2RectPos, Vector2(rectWidth, m_menuSize*2), color);

	m_curve->FlipX();
	m_curve->Draw(v2Pos+Vector2(v2CurveSize.x+rectWidth, 0), color);

	ShadowPrint(v2RectPos+Vector2(v2CurveSize.x, m_menuSize/2), text.c_str(), L"Verdana14_shadow.fnt", GS_COLOR(color.a,255,255,255));
	video->SetCameraPos(v2Cam);
	return (mouseOver && input->GetKeyState(GSK_LMOUSE) == GSKS_HIT);
}

void EditorBase::SaveAttributeToInfoFile(const string &programPath, const string &entity, const string &attrib, const string &value)
{
	wstring file = utf8::c(programPath).wstr();
	file += L"\\";
	file += _ENML_EDITOR_GENERAL_INFO;
	enml::File parseFile(enml::getStringFromAnsiFile(file));
	parseFile.add(utf8::c(entity).wstr(), utf8::c(attrib).wstr(), utf8::c(value).wstr());
	parseFile.writeToFile(file);
}

string EditorBase::GetAttributeFromInfoFile(const string &programPath, const string &entity, const string &attrib)
{
	wstring file = utf8::c(programPath).wstr();
	file += L"\\";
	file += _ENML_EDITOR_GENERAL_INFO;
	enml::File parseFile(enml::getStringFromAnsiFile(file));
	return utf8::c(parseFile.get(utf8::c(entity).wstr(), utf8::c(attrib).wstr())).str();
}

void EditorBase::CreateFileMenu()
{
	m_fileMenu.SetupMenu(m_provider->GetVideo(), m_provider->GetInput(), _S_MENU_TITLE, m_menuSize * m_menuScale, m_menuWidth * 2, true);
	m_fileMenu.AddButton(_S_NEW);
	m_fileMenu.AddButton(_S_OPEN);
	m_fileMenu.AddButton(_S_SAVE);
	m_fileMenu.AddButton(_S_SAVE_AS);
	m_fileMenu.AddButton(_S_GOTO_PROJ);
}

GSGUI_BUTTON EditorBase::PlaceFileMenu()
{
	const float shiftDown = (m_provider->GetResourcePath() == L"") ? 0.0f : m_menuSize*2;
	return m_fileMenu.PlaceMenu(Vector2(0,shiftDown));
}

void EditorBase::SetFileNameToTitle(VideoPtr video, const wchar_t *wszTitle)
{
	wstring newTitle, file;
	file = utf8::c(GetCurrentFile(false)).wc_str();
	newTitle = wszTitle;
	newTitle += L" - ";
	newTitle += file;
	video->SetWindowTitle(newTitle);
}

void EditorBase::SetCurrentFile(const char *file)
{
	m_currentFile = file;
}

bool EditorBase::AddExtension(const char *path, const char *extension, string &sOut)
{
	sOut = path;
	const int extLen = strlen(extension);
	const int len = strlen(path);
	if (strcmp(&path[len-extLen], extension) != 0)
	{
		sOut += extension;
		return true;
	}
	return false;
}

string EditorBase::GetCurrentProjectPath(const bool keepLastSlash)
{
	return ETHGlobal::GetPathName(utf8::c(m_provider->GetResourcePath()).c_str(), keepLastSlash);
}

string EditorBase::GetCurrentProject()
{
	return utf8::c(m_provider->GetResourcePath()).c_str();
}

bool EditorBase::SetCurrentProject(const char *path)
{
	m_provider->SetResourcePath(utf8::c(ETHGlobal::GetPathName(path, true)).wstr());
	return true;
}

string EditorBase::GetCurrentFile(const bool fullPath)
{
	if (fullPath)
		return m_currentFile;
	return ETHGlobal::GetFileName(m_currentFile);
}

string EditorBase::GetProgramPath()
{
	return utf8::c(m_provider->GetProgramPath()).str();
}

void EditorBase::ShadowPrint(Vector2 v2Pos, const wchar_t *text, const GS_COLOR color) const
{
	m_provider->GetVideo()->DrawBitmapText(
		v2Pos, 
		text, L"Verdana14_shadow.fnt", color
	);
}

void EditorBase::ShadowPrint(Vector2 v2Pos, const wchar_t *text, const wchar_t *font, const GS_COLOR color) const
{
	m_provider->GetVideo()->DrawBitmapText(v2Pos, text, font, color);
}

VideoPtr EditorBase::GetVideoHandler()
{
	return m_provider->GetVideo();
}

InputPtr EditorBase::GetInputHandler()
{
	return m_provider->GetInput();
}

AudioPtr EditorBase::GetAudioHandler()
{
	return m_provider->GetAudio();
}

#include <windows.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

bool DirectoryExists(const std::string& dir)
{
	if (_access(dir.c_str(), 0) == 0)
	{
		struct stat status;
		stat(dir.c_str(), &status);
		return true;
	}
	else
	{
		return false;
	}
}

bool EditorBase::OpenForm(const char *filter, const char *directory, char *szoFilePathName, char *szoFileName, const char *szSolutionPath)
{
	char fileInfo[___OUTPUT_LENGTH] = "\0";
	char fileName[___OUTPUT_LENGTH];
	string sInitDir = szSolutionPath;

	// sInitDir += "\\";
	sInitDir += Platform::AddLastSlash(directory);
	//assert(DirectoryExists(sInitDir));

	OPENFILENAMEA openFileName;
	openFileName.lStructSize       = sizeof(openFileName);
	openFileName.hwndOwner         = NULL;
	openFileName.hInstance         = 0;
	openFileName.lpstrFilter       = filter;
	openFileName.lpstrCustomFilter = NULL;
	openFileName.nMaxCustFilter    = 0;
	openFileName.nFilterIndex      = 1;
	openFileName.lpstrFile         = fileInfo;
	openFileName.nMaxFile          = ___OUTPUT_LENGTH;
	openFileName.lpstrFileTitle    = fileName;
	openFileName.nMaxFileTitle     = ___OUTPUT_LENGTH;
	openFileName.lpstrInitialDir   = sInitDir.c_str();
	openFileName.lpstrTitle        = NULL;
	openFileName.Flags             = (OFN_FILEMUSTEXIST);
	openFileName.lpstrDefExt       = NULL;

	if (!GetOpenFileNameA(&openFileName))
	{
		return false;
	}

	_ETH_SAFE_strcpy(szoFileName, fileName);
	_ETH_SAFE_strcpy(szoFilePathName, fileInfo);
	return true;
}

bool EditorBase::SaveForm(const char *filter, const char *szDir, char *szoFilePathName,
					   char *szoFileName, const char *szSolutionPath)
{
	char fileInfo[___OUTPUT_LENGTH] = "\0";
	char fileName[___OUTPUT_LENGTH];
	string sInitDir = szSolutionPath;

	// sInitDir += "\\";
	sInitDir += Platform::AddLastSlash(szDir);
	assert(DirectoryExists(sInitDir));

	OPENFILENAMEA saveFileName;
	saveFileName.lStructSize       = sizeof(saveFileName);
	saveFileName.hwndOwner         = NULL;
	saveFileName.hInstance         = 0;
	saveFileName.lpstrFilter       = filter;
	saveFileName.lpstrCustomFilter = NULL;
	saveFileName.nMaxCustFilter    = 0;
	saveFileName.nFilterIndex      = 1;
	saveFileName.lpstrFile         = fileInfo;
	saveFileName.nMaxFile          = ___OUTPUT_LENGTH;
	saveFileName.lpstrFileTitle    = fileName;
	saveFileName.nMaxFileTitle     = ___OUTPUT_LENGTH;
	saveFileName.lpstrInitialDir   = sInitDir.c_str();
	saveFileName.lpstrTitle        = NULL;
	saveFileName.Flags             = (OFN_OVERWRITEPROMPT);
	saveFileName.lpstrDefExt       = NULL;

	if (!GetSaveFileNameA(&saveFileName))
	{
		return false;
	}

	_ETH_SAFE_strcpy(szoFileName, fileName);
	_ETH_SAFE_strcpy(szoFilePathName, fileInfo);
	return true;
}

namespace ETHGlobal 
{
	bool CopyFileToProject(const wstring &currentPath, const wstring &filePath, const wstring &destPath, const Platform::FileManagerPtr& fileManager)
	{
		const wstring fileName = ETHGlobal::GetFileName(filePath);
		if (ETHGlobal::FileExists(currentPath+L"\\"+destPath+fileName, fileManager))
		{
			return true;
		}
		else
		{
			ETHGlobal::_MoveFile(filePath, currentPath+L"\\"+destPath+fileName, false);
		}
		return true;
	}

	void ErrorMessageBox(const char *szMessage, const char *szTitle)
	{
		MessageBox(
			NULL, utf8::c(szMessage).wc_str(), utf8::c(szTitle).wc_str(), MB_OK|MB_ICONERROR
		);
	}

	bool _MoveFile(const wstring &source, const wstring &dest, const bool overwrite)
	{
		ifstream ifs(utf8::c(source).c_str(), ios::binary);
		if (!ifs.is_open())
		{
			wcerr << "Couldn't copy the file " << source << endl;
			return false;
		}

		if (!overwrite)
		{
			ifstream exist(utf8::c(dest).c_str(), ios::binary);
			if (exist.is_open())
			{
				exist.close();
				ifs.close();
				return true;
			}
		}

		ofstream ofs(utf8::c(dest).c_str(), ios::binary);
		if (!ofs.is_open())
		{
			ifs.close();
			wcerr << L"Couldn't copy the file " << dest << endl;
			return false;
		}

		ofs << ifs.rdbuf();
		ofs.close();
		ifs.close();
		return true;
	}
} 
// namespace ETHGlobal

