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

#ifndef CUSTOM_DATA_EDITOR_HEADER_38502750432
#define CUSTOM_DATA_EDITOR_HEADER_38502750432

#include "IEditorPlugin.h"
#include "gsgui.h"

enum CUSTOM_DATA_EDITOR_STATE
{
	CDES_IDLE = 0,
	CDES_ADDING_FLOAT = 1,
	CDES_ADDING_INT = 2,
	CDES_ADDING_UINT = 3,
	CDES_ADDING_STRING = 4,
};

class CustomDataEditor : public IEditorPlugin
{
public:
	CustomDataEditor();
	void LoadEditor(EditorBase *pEditor);
	float DoEditor(const Vector2 &v2Pos, ETHEntity* pEntity, EditorBase *pEditor);
	void ShowInScreenCustomData(const ETHEntity* pEntity, EditorBase *pEditor) const;
	bool IsCursorAvailable() const;
	bool IsFieldActive() const;
	void Rebuild(const ETHEntity* pEntity, EditorBase *pEditor);

private:
	float PlaceButtons(const Vector2 &v2Pos, const ETHEntity* pEntity, EditorBase *pEditor);
	void InputVariableName(ETHEntity* pEntity, EditorBase *pEditor);
	void EditVariable(ETHEntity* pEntity, EditorBase *pEditor);
	void DrawInputFieldRect(const Vector2 &v2Pos,
		const GS_GUI* pGui, EditorBase *pEditor,
		const str_type::char_t *text) const;

	int m_lastEntityID;
	str_type::string m_lastData;
	GSGUI_BUTTONLIST m_customDataButtonList;
	GSGUI_DROPDOWN m_options;
	CUSTOM_DATA_EDITOR_STATE m_cdesState;
	GSGUI_STRING_LINE_INPUT m_inVariableName;
	GSGUI_STRING_LINE_INPUT m_inValueInput;
};

#endif