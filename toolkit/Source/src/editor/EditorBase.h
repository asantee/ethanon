#ifndef __EDITOR_INTERFACE_OBJECT_49383025432
#define __EDITOR_INTERFACE_OBJECT_49383025432

#include "gsgui.h"
#include "../engine/Resource/ETHResourceProvider.h"
#include <string>

#include "EditorCommon.h"

#define _BASEEDITOR_DEFAULT_UNTITLED_FILE "Untitled"

#define _S_MENU_TITLE GS_L("File")
#define _S_NEW GS_L("New")
#define _S_OPEN GS_L("Open")
#define _S_SAVE GS_L("Save")
#define _S_SAVE_AS GS_L("Save as...")
#define _S_GOTO_PROJ GS_L("Go to project manager")

#define _S_USE_PS GS_L("Pixel shaded")
#define _S_USE_VS GS_L("Vertex shaded")
#define _S_GENERATE_LIGHTMAPS GS_L("Update lightmaps")
#define _S_SAVE_LIGHTMAPS GS_L("Save lightmaps to file")
#define _S_UPDATE_ENTITIES GS_L("Update entities")
#define _S_LOCK_STATIC GS_L("Lock static entities")
#define _S_USE_STATIC_LIGHTMAPS GS_L("Use static lightmaps")
#define _S_SHOW_CUSTOM_DATA GS_L("Show custom data")
#define _S_SHOW_ADVANCED_OPTIONS GS_L("Show advanced options")
#define _S_TOGGLE_FLIPX GS_L("Flip X")
#define _S_TOGGLE_FLIPY GS_L("Flip Y")
#define _S_TOGGLE_STATIC_DYNAMIC GS_L("Toggle static/dynamic")

const Vector2 ETH_DEFAULT_ZDIRECTION(0,-1);

#define ___OUTPUT_LENGTH 2048

class EditorBase
{
public:
	EditorBase(ETHResourceProviderPtr provider);

	bool SetCurrentProject(const char *path);
	std::string GetCurrentProject();
	std::string GetCurrentProjectPath(const bool keepLastSlash);
	virtual void Clear() = 0;
	virtual bool ProjectManagerRequested() = 0;
	virtual void LoadEditor() = 0;
	virtual std::string DoEditor(SpritePtr pNextAppButton, const float lastFrameElapsedTime) = 0;
	virtual void StopAllSoundFXs() = 0;
	virtual void ReloadFiles();
	virtual void UpdateInternalData();
	void SetCurrentFile(const char *file);
	std::string GetCurrentFile(const bool fullPath);
	std::string GetProgramPath();

	float GetMenuSize() const;
	float GetMenuWidth() const;
	Color GetBGColor() const;

	bool DrawTab(VideoPtr video, InputPtr input, const Vector2 &v2Pos, const float width, const std::string &text, Color color);

	void SaveAttributeToInfoFile(const std::string &entity, const std::string &attrib, const std::string &value);
	std::string GetAttributeFromInfoFile(const std::string &entity, const std::string &attrib);

	void ShadowPrint(Vector2 v2Pos, const str_type::char_t *text, const Color& color = gs2d::constant::WHITE) const;
	void ShadowPrint(Vector2 v2Pos, const str_type::char_t *text, const str_type::char_t *font, const Color& color) const;

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
	bool AddExtension(const char *path, const char *extension, std::string &sOut);
	void SetFileNameToTitle(VideoPtr video, const str_type::char_t *wszTitle);
	bool OpenForm(const FILE_FORM_FILTER& filter, const char *directory, char *szoFilePathName, char *szoFileName);
	bool SaveForm(const FILE_FORM_FILTER& filter, const char *directory, char *szoFilePathName, char *szoFileName);

private:
	std::string m_currentFile;
};

namespace ETHGlobal 
{
	bool CopyFileToProject(const std::string &currentPath, const std::string &filePath, const std::string &destPath, const Platform::FileManagerPtr& fileManager);
	bool _MoveFile(const std::string &source, const std::string &dest, const bool overwrite);
	bool PointInRect(const Vector2 &p, const Vector2 &pos0, const Vector2 &size0);
	std::string Vector3ToString(const Vector3 &v3);
}

#endif
