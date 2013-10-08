/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#include "CustomDataEditor.h"

#include <string>
#include <sstream>
#include <map>

CustomDataEditor::CustomDataEditor()
{
	m_lastEntityID =-2;
	m_cdesState = CDES_IDLE;
	m_lastData.clear();
}

void CustomDataEditor::LoadEditor(EditorBase *pEditor)
{
	m_options.SetupMenu(pEditor->GetVideoHandler(), pEditor->GetInputHandler(),
		GS_L("+ New variable"), pEditor->GetMenuSize(), pEditor->GetMenuWidth()*2, true);
	m_options.AddButton(GS_L("int"));
	m_options.AddButton(GS_L("uint"));
	m_options.AddButton(GS_L("float"));
	m_options.AddButton(GS_L("string"));
}

float CustomDataEditor::DoEditor(const Vector2 &v2Pos, ETHEntity* pEntity, EditorBase *pEditor)
{
	if (!pEntity)
	{
		m_inVariableName.SetActive(false);
		m_inValueInput.SetActive(false);
		m_cdesState = CDES_IDLE;
		return 0.0f;
	}

	float fR = 0.0f;
	VideoPtr video = pEditor->GetVideoHandler();
	InputPtr input = pEditor->GetInputHandler();

	pEditor->ShadowPrint(v2Pos+Vector2(0,fR), GS_L("Custom data:"));
	fR += pEditor->GetMenuSize();

	const GSGUI_BUTTON r = m_options.PlaceMenu(v2Pos+Vector2(0,fR));
	fR += m_options.GetCurrentHeight();
	fR += pEditor->GetMenuSize()/2;

	// if the state is set to input a new variable name...
	if (m_cdesState >= CDES_ADDING_FLOAT && m_cdesState <= CDES_ADDING_STRING)
	{
		InputVariableName(pEntity, pEditor);
	}

	// edit values
	EditVariable(pEntity, pEditor);

	// checks if the user pressed the "Add variable" button and handles it
	if (r.text != GS_L(""))
	{
		m_inVariableName.SetupMenu(video, input, pEditor->GetMenuSize(), pEditor->GetMenuWidth()*4.0f, 32, true);
		if (r.text == GS_L("int"))
		{
			m_cdesState = CDES_ADDING_INT;
		}
		else if (r.text == GS_L("uint"))
		{
			m_cdesState = CDES_ADDING_UINT;
		}
		else if (r.text == GS_L("float"))
		{
			m_cdesState = CDES_ADDING_FLOAT;
		}
		else if (r.text == GS_L("string"))
		{
			m_cdesState = CDES_ADDING_STRING;
		}
	}

	fR += PlaceButtons(v2Pos+Vector2(0,fR), pEntity, pEditor);
	fR += pEditor->GetMenuSize()/2;

	if (!pEntity->HasCustomData() && m_cdesState == CDES_IDLE)
	{
		Rebuild(pEntity, pEditor);
		m_inVariableName.SetActive(false);
		m_inValueInput.SetActive(false);
	}

	return fR;
}

void CustomDataEditor::InputVariableName(ETHEntity* pEntity, EditorBase *pEditor)
{
	VideoPtr video = pEditor->GetVideoHandler();
	const Vector2 v2Pos = video->GetScreenSizeF()/2.0f - Vector2(m_inVariableName.GetWidth()/2.0f, 0.0f);

	if (m_inVariableName.IsActive())
	{
		DrawInputFieldRect(v2Pos, &m_inVariableName, pEditor, GS_L("Enter variable name"));
		m_inVariableName.PlaceInput(v2Pos);

		str_type::stringstream ss;
		ss << ETHCustomDataManager::GetDataName(m_cdesState);
		pEditor->ShadowPrint(v2Pos-Vector2(0.0f,m_inVariableName.GetSize()), ss.str().c_str(),
			GS_L("Verdana14_shadow.fnt"), gs2d::constant::BLACK);

		// if it has just been unactivated
		if (!m_inVariableName.IsActive())
		{
			if (m_inVariableName.GetValue() != GS_L(""))
			{
				switch (m_cdesState)
				{
				case CDES_ADDING_INT:
					pEntity->SetInt(m_inVariableName.GetValue(), 0);
					break;
				case CDES_ADDING_UINT:
					pEntity->SetUInt(m_inVariableName.GetValue(), 0);
					break;
				case CDES_ADDING_FLOAT:
					pEntity->SetFloat(m_inVariableName.GetValue(), 0.0f);
					break;
				case CDES_ADDING_STRING:
					pEntity->SetString(m_inVariableName.GetValue(), GS_L("none"));
					break;
				default:
					break;
				}
				Rebuild(pEntity, pEditor);
			}
			m_cdesState = CDES_IDLE;
		}
	}
}

void CustomDataEditor::ShowInScreenCustomData(const ETHEntity* pEntity, EditorBase *pEditor) const
{
	std::map<str_type::string, ETHCustomDataPtr> dataMap;
	pEntity->GetCustomDataManager()->CopyMap(dataMap);

	const Vector2 v2Pos(pEntity->GetPositionXY()-Vector2(0,pEntity->GetPosition().z)
		- pEditor->GetVideoHandler()->GetCameraPos());
	pEditor->ShadowPrint(v2Pos, pEntity->GetCustomDataManager()->GetDebugStringData().c_str());
}

float CustomDataEditor::PlaceButtons(const Vector2 &v2Pos, const ETHEntity* pEntity, EditorBase *pEditor)
{
	VideoPtr video = pEditor->GetVideoHandler();
	InputPtr input = pEditor->GetInputHandler();
	if (pEntity->HasCustomData())
	{
		m_customDataButtonList.PlaceMenu(v2Pos);

		// if the cursor is over, show message
		if (m_customDataButtonList.IsMouseOver())
		{
			pEditor->ShadowPrint(input->GetCursorPositionF(video)-Vector2(0,pEditor->GetMenuSize()), GS_L("Click to edit"));
		}
	}
	else
	{
		m_customDataButtonList.Clear();
	}
	return static_cast<float>(m_customDataButtonList.GetNumButtons())*pEditor->GetMenuSize();
}

bool CustomDataEditor::IsCursorAvailable() const
{
	if (m_inVariableName.IsActive() || m_inValueInput.IsActive())
	{
		return false;
	}
	return (!m_options.IsMouseOver() && (m_cdesState == CDES_IDLE) && !m_customDataButtonList.IsMouseOver());
}

bool CustomDataEditor::IsFieldActive() const
{
	if (m_inVariableName.IsActive())
		return true;
	if (m_inValueInput.IsActive())
		return true;
	return false;
}

void CustomDataEditor::Rebuild(const ETHEntity* pEntity, EditorBase *pEditor)
{
	m_customDataButtonList.Clear();
	m_customDataButtonList.SetupMenu(pEditor->GetVideoHandler(), pEditor->GetInputHandler(),
		pEditor->GetMenuSize(), pEditor->GetMenuWidth()*2, true, true, false);

	std::map<str_type::string, ETHCustomDataPtr> dataMap;
	pEntity->GetCustomDataManager()->CopyMap(dataMap);

	for (std::map<str_type::string, ETHCustomDataPtr>::const_iterator iter = dataMap.begin();
		 iter != dataMap.end(); ++iter)
	{
		const ETHCustomDataPtr& data = iter->second;
		const str_type::string& name = iter->first;
		str_type::stringstream ss;
		ss << GS_L(" = ");
		bool editable = true;
		switch (data->GetType())
		{
		case ETHCustomData::DT_INT:
			ss << data->GetInt();
			break;
		case ETHCustomData::DT_UINT:
			ss << data->GetUInt();
			break;
		case ETHCustomData::DT_FLOAT:
			ss << data->GetFloat();
			break;
		case ETHCustomData::DT_STRING:
			ss << GS_L("\"") << data->GetString() << GS_L("\"");
			break;
		default:
			editable = false;
		};
		if (editable)
			m_customDataButtonList.AddButton(name.c_str(), false, ss.str().c_str());
	}
}

void CustomDataEditor::EditVariable(ETHEntity* pEntity, EditorBase *pEditor)
{
	if (m_cdesState != CDES_IDLE)
		return;

	VideoPtr video = pEditor->GetVideoHandler();
	InputPtr input = pEditor->GetInputHandler();

	GSGUI_BUTTON r = m_customDataButtonList.GetFirstActiveButton();

	if (m_lastData != r.text || pEntity->GetID() != m_lastEntityID)
	{
		m_inValueInput.SetupMenu(
			video, input, pEditor->GetMenuSize(),
			pEditor->GetMenuWidth()*4.0f, 144, false, 
			pEntity->GetCustomDataManager()->GetValueAsString(r.text)
		);
		m_lastData = r.text;
		m_lastEntityID = pEntity->GetID();
	}

	if (r.text != GS_L(""))
	{
		m_inValueInput.SetActive(true);
		const Vector2 v2Pos = video->GetScreenSizeF()/2.0f - Vector2(m_inValueInput.GetWidth()/2.0f, 0.0f);

		DrawInputFieldRect(v2Pos, &m_inValueInput, pEditor, GS_L("Leave blank to erase variable"));

		const ETHCustomData::DATA_TYPE type = pEntity->CheckCustomData(r.text);
		switch (type)
		{
		case ETHCustomData::DT_INT:
			pEntity->SetInt(r.text, ETHGlobal::ParseInt(m_inValueInput.PlaceInput(v2Pos).c_str()));
			break;
		case ETHCustomData::DT_UINT:
			pEntity->SetUInt(r.text, ETHGlobal::ParseUInt(m_inValueInput.PlaceInput(v2Pos).c_str()));
			break;
		case ETHCustomData::DT_STRING:
			pEntity->SetString(r.text, m_inValueInput.PlaceInput(v2Pos));
			break;
		case ETHCustomData::DT_FLOAT:
			pEntity->SetFloat(r.text, ETHGlobal::ParseFloat(m_inValueInput.PlaceInput(v2Pos).c_str()));
			break;
		default:
			break;
		};
		str_type::stringstream ss;
		ss << ETHCustomDataManager::GetDataName(type) << GS_L(" ") << r.text << GS_L(":");
		pEditor->ShadowPrint(v2Pos-Vector2(0.0f,m_inValueInput.GetSize()), ss.str().c_str(),
			GS_L("Verdana14_shadow.fnt"), gs2d::constant::BLACK);
	}
	else
	{
		Rebuild(pEntity, pEditor);
	}

	if (!m_inValueInput.IsActive())
	{
		if (m_inValueInput.GetValue() == GS_L(""))
		{
			pEntity->EraseData(r.text);
			Rebuild(pEntity, pEditor);
		}
		m_customDataButtonList.DeactivateButton(r.text);
	}
}

void CustomDataEditor::DrawInputFieldRect(const Vector2 &v2Pos,
										   const GS_GUI *pGui,
										   EditorBase *pEditor,
										   const str_type::char_t *text) const
{
	VideoPtr video = pEditor->GetVideoHandler();
	const Vector2 v2BorderSize(5, 5);
	const Vector2 v2FinalPos(v2Pos - v2BorderSize-Vector2(0, pGui->GetSize()));
	const Vector2 v2Size(Vector2(pGui->GetWidth(), pGui->GetSize() * 2 + pEditor->GetMenuSize()) + v2BorderSize * 2);
	const GSGUI_STYLE* style = pGui->GetGUIStyle();
	video->DrawRectangle(v2FinalPos, v2Size, style->active_top, style->active_top, style->active_bottom, style->active_bottom);
	pEditor->ShadowPrint(Vector2(v2FinalPos.x, v2FinalPos.y + v2Size.y - pEditor->GetMenuSize()), text, gs2d::constant::BLACK);
}
