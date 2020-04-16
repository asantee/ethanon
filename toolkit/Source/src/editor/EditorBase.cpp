#include "../engine/Platform/FileListing.h"
#include "../engine/Util/ETHASUtil.h"
#include "../engine/Resource/ETHDirectories.h"

#include <Enml/Enml.h>

#include "EditorBase.h"
#include "EditorCommon.h"


#define _ENML_EDITOR_GENERAL_INFO GS_L("userdata.enml")

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

Color EditorBase::GetBGColor() const
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

void ShadowPrint(VideoPtr video, Vector2 v2Pos, const str_type::char_t *text, const str_type::char_t *font, GS_DWORD color)
{
	video->DrawBitmapText(v2Pos, text, font, color);
}
bool EditorBase::DrawTab(VideoPtr video, InputPtr input, const Vector2 &v2Pos, const float width, const std::string &text, Color color)
{
	video->SetAlphaMode(Video::AM_PIXEL);
	video->SetVertexShader(ShaderPtr());
	video->SetPixelShader(ShaderPtr());
	video->SetZBuffer(false);
	video->SetZWrite(false);

	const Vector2 v2Cam = video->GetCameraPos();
	video->SetCameraPos(Vector2(0,0));
	bool mouseOver = false;
	if ((mouseOver = ETHGlobal::PointInRect(input->GetCursorPositionF(video), v2Pos+Vector2(width/2, m_menuSize), Vector2(width, m_menuSize))))
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

	ShadowPrint(v2RectPos+Vector2(v2CurveSize.x, m_menuSize/2), text.c_str(), GS_L("Verdana14_shadow.fnt"), Color(color.a,255,255,255));
	video->SetCameraPos(v2Cam);
	return (mouseOver && input->GetKeyState(GSK_LMOUSE) == GSKS_HIT);
}

void EditorBase::SaveAttributeToInfoFile(
	const std::string &entity,
	const std::string &attrib,
	const std::string &value)
{
	std::string file = m_provider->GetFileIOHub()->GetExternalStorageDirectory() + _ENML_EDITOR_GENERAL_INFO;
	enml::File parseFile(enml::GetStringFromAnsiFile(file));
	parseFile.Add(entity, attrib, value);
	parseFile.WriteToFile(file);
}

std::string EditorBase::GetAttributeFromInfoFile(
	const std::string &entity,
	const std::string &attrib)
{
	std::string file = m_provider->GetFileIOHub()->GetExternalStorageDirectory() + _ENML_EDITOR_GENERAL_INFO;
	if (!m_provider->GetFileIOHub()->GetFileManager()->FileExists(file))
	{
		file = m_provider->GetFileIOHub()->GetProgramDirectory() + GS_L("editor.enml");
	}
	enml::File parseFile(enml::GetStringFromAnsiFile(file));
	return parseFile.Get(entity, attrib);
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
	const float shiftDown = (m_provider->GetFileIOHub()->GetResourceDirectory() == GS_L("")) ? 0.0f : m_menuSize*2;
	return m_fileMenu.PlaceMenu(Vector2(0,shiftDown));
}

void EditorBase::SetFileNameToTitle(VideoPtr video, const str_type::char_t *wszTitle)
{
	std::string newTitle, file;
	file = GetCurrentFile(false);
	newTitle = wszTitle;
	newTitle += GS_L(" - ");
	newTitle += file;
	video->SetWindowTitle(newTitle);
}

void EditorBase::SetCurrentFile(const char *file)
{
	m_currentFile = file;
}

bool EditorBase::AddExtension(const char *path, const char *extension, std::string &sOut)
{
	sOut = ETHGlobal::AppendExtensionIfNeeded(path, extension);
	return (std::string(extension) != sOut);
}

std::string EditorBase::GetCurrentProjectPath(const bool keepLastSlash)
{
	std::string r = Platform::GetFileDirectory(m_provider->GetFileIOHub()->GetResourceDirectory().c_str());
	if (!keepLastSlash)
		r = r.substr(0, r.size() - 1);
	return r;
}

std::string EditorBase::GetCurrentProject()
{
	return m_provider->GetFileIOHub()->GetResourceDirectory();
}

bool EditorBase::SetCurrentProject(const char *path)
{
	m_provider->GetFileIOHub()->SetResourceDirectory(Platform::GetFileDirectory(path));
	return true;
}

std::string EditorBase::GetCurrentFile(const bool fullPath)
{
	if (fullPath)
		return m_currentFile;
	return Platform::GetFileName(m_currentFile);
}

std::string EditorBase::GetProgramPath()
{
	return m_provider->GetFileIOHub()->GetProgramDirectory();
}

void EditorBase::ShadowPrint(Vector2 v2Pos, const str_type::char_t *text, const Color& color) const
{
	m_provider->GetVideo()->DrawBitmapText(
		v2Pos, 
		text, GS_L("Verdana14_shadow.fnt"), color
	);
}

void EditorBase::ShadowPrint(Vector2 v2Pos, const str_type::char_t *text, const str_type::char_t *font, const Color& color) const
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

namespace ETHGlobal 
{
bool CopyFileToProject(
	const std::string &currentPath,
	const std::string &filePath,
	const std::string &destPath,
	const Platform::FileManagerPtr& fileManager)
{
	std::stringstream slash; slash << Platform::GetDirectorySlashA();
	const std::string fileName = Platform::GetFileName(filePath);
	if (ETHGlobal::FileExists(currentPath + slash.str() + destPath + fileName, fileManager))
	{
		return true;
	}
	else
	{
		ETHGlobal::_MoveFile(filePath, currentPath + slash.str() + destPath + fileName, false);
	}
	return true;
}

bool _MoveFile(const std::string &source, const std::string &dest, const bool overwrite)
{
	std::ifstream ifs(source.c_str(), std::ios::binary);
	if (!ifs.is_open())
	{
		GS2D_CERR << "Couldn't copy file " << source << std::endl;
		return false;
	}

	if (!overwrite)
	{
		std::ifstream exist(dest.c_str(), std::ios::binary);
		if (exist.is_open())
		{
			exist.close();
			ifs.close();
			return true;
		}
	}

	std::ofstream ofs(dest.c_str(), std::ios::binary);
	if (!ofs.is_open())
	{
		ifs.close();
		GS2D_CERR << GS_L("Couldn't copy file ") << dest << std::endl;
		return false;
	}

	ofs << ifs.rdbuf();
	ofs.close();
	ifs.close();
	return true;
}

bool PointInRect(const Vector2 &p, const Vector2 &pos0, const Vector2 &size0)
{
	const Vector2 halfSize0 = size0/2;
	const Vector2 v2Min0 = pos0-halfSize0;
	const Vector2 v2Max0 = pos0+halfSize0;

	if (p.x > v2Max0.x)
		return false;
	if (p.y > v2Max0.y)
		return false;

	if (p.x < v2Min0.x)
		return false;
	if (p.y < v2Min0.y)
		return false;

	return true;
}

std::string Vector3ToString(const Vector3 &v3)
{
	std::stringstream ss;
	ss << GS_L("(") << v3.x << GS_L(", ") << v3.y << GS_L(", ") << v3.z << GS_L(")");
	return ss.str();
}

} // namespace ETHGlobal
