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
	std::string m_lastData;
	GSGUI_BUTTONLIST m_customDataButtonList;
	GSGUI_DROPDOWN m_options;
	CUSTOM_DATA_EDITOR_STATE m_cdesState;
	GSGUI_STRING_LINE_INPUT m_inVariableName;
	GSGUI_STRING_LINE_INPUT m_inValueInput;
};

#endif