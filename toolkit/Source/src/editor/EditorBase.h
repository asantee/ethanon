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

#ifndef __EDITOR_INTERFACE_OBJECT_49383025432
#define __EDITOR_INTERFACE_OBJECT_49383025432

#include "gsgui.h"
#include "../engine/Resource/ETHResourceProvider.h"
#include <string>

using namespace std; // TODO: remove it from header

#define _BASEEDITOR_DEFAULT_UNTITLED_FILE "Untitled"

#define _S_MENU_TITLE L"File"
#define _S_NEW L"New"
#define _S_OPEN L"Open"
#define _S_SAVE L"Save"
#define _S_SAVE_AS L"Save as..."
#define _S_GOTO_PROJ L"Go to project manager"

#define _S_USE_PS L"Pixel shaded"
#define _S_USE_VS L"Vertex shaded"
#define _S_GENERATE_LIGHTMAPS L"Update lightmaps"
#define _S_UPDATE_ENTITIES L"Update entities"
#define _S_LOCK_STATIC L"Lock static entities"
#define _S_USE_STATIC_LIGHTMAPS L"Use static lightmaps"
#define _S_SHOW_CUSTOM_DATA L"Show custom data"
#define _S_SHOW_ADVANCED_OPTIONS L"Show advanced options"
#define _S_TOGGLE_STATIC_DYNAMIC L"Toggle static/dynamic"

const Vector2 ETH_DEFAULT_ZDIRECTION(0,-1);

#define ___OUTPUT_LENGTH 2048

class EditorBase
{
public:
	EditorBase(ETHResourceProviderPtr provider);

	bool SetCurrentProject(const char *path);
	string GetCurrentProject();
	string GetCurrentProjectPath(const bool keepLastSlash);
	virtual void Clear() = 0;
	virtual bool ProjectManagerRequested() = 0;
	virtual void LoadEditor() = 0;
	virtual string DoEditor(SpritePtr pNextAppButton) = 0;
	virtual void StopAllSoundFXs() = 0;
	virtual void ReloadFiles();
	virtual void UpdateInternalData();
	void SetCurrentFile(const char *file);
	string GetCurrentFile(const bool fullPath);
	string GetProgramPath();

	float GetMenuSize() const;
	float GetMenuWidth() const;
	Color GetBGColor() const;

	bool DrawTab(VideoPtr video, InputPtr input, const Vector2 &v2Pos, const float width, const wstring &text, Color color);

	void SaveAttributeToInfoFile(const string &programPath, const string &entity, const string &attrib, const string &value);
	string GetAttributeFromInfoFile(const string &programPath, const string &entity, const string &attrib);

	void ShadowPrint(Vector2 v2Pos, const wchar_t *text, const Color& color = gs2d::constant::WHITE) const;
	void ShadowPrint(Vector2 v2Pos, const wchar_t *text, const wchar_t *font, const Color& color) const;

	VideoPtr GetVideoHandler();
	InputPtr GetInputHandler();
	AudioPtr GetAudioHandler();

protected:
	ETHResourceProviderPtr m_provider;
	GSGUI_DROPDOWN m_fileMenu;
	SpritePtr m_curve;
	bool m_untitled;
	const float m_menuSize;
	const float m_menuWidth;
	const float m_menuScale;
	Color m_background;

	bool IsFileMenuActive() const;
	bool IsMouseOverFileMenu() const;
	GSGUI_BUTTON PlaceFileMenu();
	virtual void CreateFileMenu();
	bool AddExtension(const char *path, const char *extension, string &sOut);
	void SetFileNameToTitle(VideoPtr video, const wchar_t *wszTitle);
	bool OpenForm(const char *filter, const char *directory, char *szoFilePathName,
				  char *szoFileName, const char *szSolutionPath);
	bool SaveForm(const char *filter, const char *directory, char *szoFilePathName,
				  char *szoFileName, const char *szSolutionPath);

private:
	string m_currentFile;
};

namespace ETHGlobal 
{
	bool CopyFileToProject(const wstring &currentPath, const wstring &filePath, const wstring &destPath, const Platform::FileManagerPtr& fileManager);
	void ErrorMessageBox(const char *szMessage, const char *szTitle);
	bool _MoveFile(const std::wstring &source, const std::wstring &dest, const bool overwrite);
	bool PointInRect(const Vector2 &p, const Vector2 &pos0, const Vector2 &size0);
	str_type::string Vector3ToString(const Vector3 &v3);
}

#endif
