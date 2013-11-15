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

#include "gsgui.h"
#include <sstream>
#include "EditorCommon.h"
#include <Platform/Platform.h>

///////////////////////////////////////////////////////////////
// string input
///////////////////////////////////////////////////////////////
GSGUI_STRING_INPUT::GSGUI_STRING_INPUT()
{
	cursor = 0;
	blinkTime = 200;
	lastBlink = 0;
	showingCarret = 1;
	numbersOnly = false;
}

void GSGUI_STRING_INPUT::PlaceInput(const Vector2& pos, const str_type::string& font, const unsigned int nMaxChars, 
				const float size, const Color& dwColor, VideoPtr video,
				InputPtr input)
{
	const unsigned long time = video->GetElapsedTime();
	if ((time - lastBlink) > blinkTime)
	{
		showingCarret = showingCarret == 0 ? 1 : 0;
		lastBlink = video->GetElapsedTime();
	}

	// pick the text cursor position with a mouse click
	if (input->GetLeftClickState() == GSKS_HIT)
	{
		cursor = video->FindClosestCarretPosition(font, ss, pos, input->GetCursorPositionF(video));
	}
	
	// don't let the cursor exceed the text range
	cursor = Min(cursor, static_cast<std::size_t>(ss.length()));

	str_type::string lastInput = input->GetLastCharInput();
	if (!lastInput.empty() && ss.length() < nMaxChars)
	{
		if (numbersOnly)
		{
			if (lastInput.length() == 1)
			{
				const str_type::char_t lastChar = lastInput[0];
				if ((lastChar < 'a' || lastChar > 'z') &&
					(lastChar < 'A' || lastChar > 'Z'))
				{
					ss.insert(cursor, 1, lastChar);
				}
			}
		}
		else
		{
			ss.insert(cursor, lastInput);
		}
		cursor++;
	}

	if (input->GetKeyState(GSK_BACK) == GSKS_HIT)
	{
		if (cursor > 0)
		{
			ss.erase(cursor-1, 1);
			cursor--;
		}
	}
	
	if (input->GetKeyState(GSK_DELETE) == GSKS_HIT)
	{
		if (cursor < ss.length())
		{
			ss.erase(cursor, 1);
		}
	}
	
	if (input->GetKeyState(GSK_LEFT) == GSKS_HIT)
	{
		if (cursor > 0)
			cursor--;
	}
	if (input->GetKeyState(GSK_RIGHT) == GSKS_HIT)
	{
		cursor = Min(++cursor, static_cast<std::size_t>(ss.length()));
	}
	if (input->GetKeyState(GSK_END) == GSKS_HIT)
	{
		SendCursorToEnd();
	}

	const Vector2 cursorPosition = video->ComputeCarretPosition(font, ss, static_cast<unsigned int>(cursor));
	video->DrawBitmapText(pos, ss, font, dwColor);
	if (showingCarret==1)
		video->DrawRectangle(cursorPosition+pos-Vector2(2,0), Vector2(2,size), gs2d::constant::BLACK, gs2d::constant::BLACK, gs2d::constant::BLACK, gs2d::constant::BLACK, 0.0f);
}

void GSGUI_STRING_INPUT::Place(const Vector2& pos, const str_type::string& font,
				const float size, const Color& dwColor, VideoPtr video)
{
	str_type::string outputString = ss;
	video->DrawBitmapText(pos, outputString, font, dwColor);
	SendCursorToEnd();
}

str_type::string GSGUI_STRING_INPUT::GetString() const
{
	return ss;
}

void GSGUI_STRING_INPUT::SetString(const str_type::string &inS)
{
	ss = inS;
}

void GSGUI_STRING_INPUT::NumbersOnly(const bool b)
{
	numbersOnly = b;
}

void GSGUI_STRING_INPUT::SendCursorToEnd()
{
	cursor = ss.length();
}

///////////////////////////////////////////////////////////////
// GUI style
///////////////////////////////////////////////////////////////
GSGUI_STYLE::GSGUI_STYLE()
{
	outline.SetColor(255,0,0,0);

	inactive_top.SetColor(192,155,155,155);
	inactive_bottom.SetColor(192,200,200,200);

	active_top.SetColor(255,255,255,255);
	active_bottom.SetColor(255,170,170,170);

	focused_top.SetColor(232,200,200,200);
	focused_bottom.SetColor(232,232,232,232);

	active_text.SetColor(255,0,0,0);
	inactive_text.SetColor(196,0,0,0);
	text_shadow.SetColor(48, 0,0,0);
}

///////////////////////////////////////////////////////////////
// GUI button
///////////////////////////////////////////////////////////////
GSGUI_BUTTON::GSGUI_BUTTON()
{
	onFocus = active = false;
	visible = true;
	text.clear();
	extText.clear();
}

///////////////////////////////////////////////////////////////
// GS_GUI
///////////////////////////////////////////////////////////////
GS_GUI::GS_GUI()
{
	Reset();
}

void GS_GUI::Reset()
{
	m_video.reset();
	m_input.reset();
	m_size = 14.0f;
	m_width = 256.0f;
	m_desc.clear();
}

str_type::string GS_GUI::GetDescription()
{
	return m_desc;
}

void GS_GUI::SetDescription(const str_type::string &desc)
{
	m_desc = desc;
}

const GSGUI_STYLE *GS_GUI::GetGUIStyle() const
{
	return &m_style;
}

float GS_GUI::GetWidth() const
{
	return m_width;

}
float GS_GUI::GetSize() const
{
	return m_size;
}

bool GS_GUI::MouseOver(Vector2 v2Pos, Vector2 v2Size)
{
	Vector2i v2iMouse = m_input->GetCursorPosition(m_video);
	Vector2 v2Mouse((float)v2iMouse.x, (float)v2iMouse.y);

	if (v2Mouse.x>=v2Pos.x && v2Mouse.x<v2Pos.x+v2Size.x &&
		v2Mouse.y>=v2Pos.y && v2Mouse.y<v2Pos.y+v2Size.y)
	{
		return true;
	}
	return false;
}

void GS_GUI::DrawOutline(Vector2 v2Pos, Vector2 v2Size)
{
	v2Size = v2Size - 1.0f;
	Vector2 v[4];
	v[0] = Vector2(v2Pos.x, v2Pos.y);
	v[1] = Vector2(v2Pos.x+v2Size.x, v2Pos.y);
	v[2] = Vector2(v2Pos.x+v2Size.x, v2Pos.y+v2Size.y);
	v[3] = Vector2(v2Pos.x, v2Pos.y+v2Size.y);
	m_video->DrawLine(v[0], v[1], m_style.outline, m_style.outline);
	m_video->DrawLine(v[1], v[2], m_style.outline, m_style.outline);
	m_video->DrawLine(v[2], v[3], m_style.outline, m_style.outline);
	m_video->DrawLine(v[3], v[0], m_style.outline, m_style.outline);
}

///////////////////////////////////////////////////////////////
// Swap image button
///////////////////////////////////////////////////////////////

void GSGUI_SWAPBUTTON::SetupButtons(VideoPtr video, InputPtr input)
{
	m_video = video;
	m_input = input;
}

GSGUI_SWAPBUTTON::GSGUI_SWAPSPRITE_CONST_PTR GSGUI_SWAPBUTTON::GetCurrentButton() const
{
	return m_current;
}

GSGUI_BUTTON GSGUI_SWAPBUTTON::PlaceMenu(Vector2 v2Pos)
{
	m_video->SetZBuffer(false);
	m_video->SetZWrite(false);
	const Vector2 size(m_current->sprite->GetBitmapSizeF());

	GS_DWORD top, bottom;

	m_mouseOver = MouseOver(v2Pos, size);
	if (m_mouseOver)
	{
		top = m_style.focused_top;
		bottom = m_style.focused_bottom;
	}
	else
	{
		top = m_style.inactive_top;
		bottom = m_style.inactive_bottom;
	}

	if (m_mouseOver)
	{
		if (m_input->GetLeftClickState() == GSKS_HIT)
		{
			GSGUI_SWAPSPRITE_PTR_LIST::iterator iter = Find(m_current->fileName);
			if (++iter == m_sprites.end())
				iter = m_sprites.begin();
			m_current = *iter;
		}
	}

	m_video->DrawRectangle(v2Pos, size, top, top, bottom, bottom);
	DrawOutline(v2Pos, size);
	m_current->sprite->Draw(v2Pos);

	GSGUI_BUTTON r;
	r.text = m_current->fileName;
	r.active = true;
	r.onFocus = m_mouseOver;
	r.value.nValue = 0;
	r.visible = true;
	r.extText.clear();
	return r;
}

GSGUI_SWAPBUTTON::GSGUI_SWAPSPRITE_PTR_LIST::iterator GSGUI_SWAPBUTTON::Find(const str_type::string &fileName)
{
	for (GSGUI_SWAPSPRITE_PTR_LIST::iterator iter = m_sprites.begin();
		iter != m_sprites.end(); ++iter)
	{
		if ((*iter)->fileName == fileName)
			return iter;
	}
	return m_sprites.end();
}

bool GSGUI_SWAPBUTTON::AddButton(const str_type::string &fileName)
{
	GSGUI_SWAPBUTTON::GSGUI_SWAPSPRITE_PTR pSprite = GSGUI_SWAPBUTTON::GSGUI_SWAPSPRITE_PTR(new GSGUI_SWAPSPRITE);
	if ((pSprite->sprite = m_video->CreateSprite(fileName)))
	{
		pSprite->fileName = Platform::GetFileName(fileName);
		m_sprites.push_back(pSprite);

		if (m_sprites.size() == 1)
		{
			m_current = *m_sprites.begin();
		}

		return true;
	}
	return false;
}

bool GSGUI_SWAPBUTTON::SetButton(const str_type::string &fileName)
{
	GSGUI_SWAPSPRITE_PTR_LIST::iterator iter = Find(fileName);
	if (iter == m_sprites.end())
		return false;
	m_current = *iter;
	return true;
}

bool GSGUI_SWAPBUTTON::IsMouseOver() const
{
	return m_mouseOver;
}

///////////////////////////////////////////////////////////////
// Drop down
///////////////////////////////////////////////////////////////
GSGUI_DROPDOWN::GSGUI_DROPDOWN()
{
	//m_video = 0;
	//m_input = 0;
	m_size = 20.0f;
	m_width = 256.0f;
	m_onClick = true;
	m_active = false;
	m_mainText.clear();
	m_mouseOver = false;
}

GSGUI_DROPDOWN::~GSGUI_DROPDOWN()
{
	Destroy();
}
void GSGUI_DROPDOWN::Destroy()
{
	if (!m_buttons.empty())
	{
		m_buttons.clear();
	}
}

bool GSGUI_DROPDOWN::IsActive() const
{
	return m_active;
}

void GSGUI_DROPDOWN::SetActive(const bool b)
{
	m_active = b;
}

void GSGUI_DROPDOWN::SetupMenu(VideoPtr video, InputPtr input, const str_type::string &mainText,
			   const float size, const float width, const bool onclick)
{
	m_video = video;
	m_input = input;
	m_size = size;
	m_width = width;
	m_onClick = onclick;
	m_mainText = mainText;
}

GSGUI_BUTTON GSGUI_DROPDOWN::PlaceMenu(Vector2 v2Pos)
{
	GSGUI_BUTTON r;

	if (!m_video)
		return r;

	m_video->SetZBuffer(false);
	m_video->SetZWrite(false);

	Vector2 v2TextAdd(m_size/4.0f, 0.0f),
			   v2ButtonSize = Vector2(m_width, m_size);
	bool overMainButton = false;
	bool overSomething = false;

	m_video->DrawRectangle(v2Pos, v2ButtonSize,
		m_style.active_top, m_style.active_top,
		m_style.active_bottom, m_style.active_bottom);
	m_video->DrawBitmapText(
		v2Pos + v2TextAdd, 
		m_mainText.c_str(), GS_L("Verdana14_shadow.fnt"), m_style.active_text
	);

	if (MouseOver(v2Pos, v2ButtonSize))
	{
		m_mouseOver = overMainButton = overSomething = true;
		DrawOutline(v2Pos, v2ButtonSize);
		if ((!m_onClick && !m_active) || m_input->GetLeftClickState() == GSKS_HIT)
			m_active = !(m_active);
	}
	else
	{
		m_mouseOver = false;
		if (m_active)
			DrawOutline(v2Pos, v2ButtonSize);
		if (m_input->GetRightClickState() == GSKS_HIT)
			m_active = false;
	}

	if (!m_buttons.empty() && m_active)
	{
		int focus=-1;
		int t=1;
		GSGUI_BUTTON_ITERATOR iter;
		for (iter = m_buttons.begin(); iter != m_buttons.end(); ++iter)
		{
			Vector2 v2ButtonPos = Vector2(v2Pos.x, v2Pos.y+((float)t*m_size));
			GS_DWORD top, bottom, text;

			iter->onFocus = MouseOver(v2ButtonPos, v2ButtonSize);

			if (iter->onFocus)
				overSomething = true;

			if (iter->onFocus)
			{
				top = m_style.focused_top;
				bottom = m_style.focused_bottom;
				text = m_style.active_text;
				focus = t;

				if (m_input->GetLeftClickState() == GSKS_HIT)
				{
					r.text = iter->text;
					top = m_style.active_top;
					bottom = m_style.active_bottom;
					m_active = false;
				}
			}
			else
			{
				top = m_style.inactive_top;
				bottom = m_style.inactive_bottom;
				text = m_style.inactive_text;
			}
			m_video->DrawRectangle(v2ButtonPos, v2ButtonSize,
				top, top, bottom, bottom);
			m_video->DrawBitmapText(
				v2ButtonPos + v2TextAdd, 
				iter->text.c_str(), GS_L("Verdana14_shadow.fnt"), text
			);
			t++;
		}

		if ((m_input->GetLeftClickState() == GSKS_HIT ||
			m_input->GetRightClickState() == GSKS_HIT))
		{
			if (m_active && focus == -1 && !overMainButton)
				m_active = false;
		}
	}

	return r;
}

void GSGUI_DROPDOWN::AddButton(const str_type::string &text)
{
	GSGUI_BUTTON button;
	button.text = text;
	m_buttons.push_back(button);
}

void GSGUI_DROPDOWN::DelButton(const str_type::string &text)
{
	if (!m_buttons.empty())
	{
		GSGUI_BUTTON_ITERATOR iter;
		for (iter = m_buttons.begin(); iter != m_buttons.end(); ++iter)
		{
			if (text == iter->text)
			{
				m_buttons.erase(iter);
				return;
			}
		}
	}
}

std::size_t GSGUI_DROPDOWN::GetNumButtons() const
{
	return m_buttons.size();
}

float GSGUI_DROPDOWN::GetCurrentHeight() const
{
	if (IsActive())
	{
		return static_cast<float>(GetNumButtons()+1)*m_size;
	}
	return m_size;
}

bool GSGUI_DROPDOWN::IsMouseOver() const
{
	return m_mouseOver;
}

///////////////////////////////////////////////////////////////
// Button list
///////////////////////////////////////////////////////////////
GSGUI_BUTTONLIST::GSGUI_BUTTONLIST()
{
	//m_video = 0;
	//m_input = 0;
	m_size = 14.0f;
	m_width = 256.0f;
	m_singleSelect = true;
	m_unselect = true;
	m_autoUnclick = false;
	m_mouseOver = false;
}
GSGUI_BUTTONLIST::~GSGUI_BUTTONLIST()
{
	Destroy();
}
void GSGUI_BUTTONLIST::Destroy()
{
	Clear();
}

void GSGUI_BUTTONLIST::Clear()
{
	if (!m_buttons.empty())
	{
		m_buttons.clear();
	}
	m_mouseOver = false;
}

void GSGUI_BUTTONLIST::SetupMenu(VideoPtr video, InputPtr input,
			   const float size, const float width, const bool single, const bool unselect,
			   const bool autounclick)
{
	m_video = video;
	m_input = input;
	m_size = size;
	m_width = width;
	m_singleSelect = single;
	m_unselect = unselect;
	m_autoUnclick = autounclick;
}

void GSGUI_BUTTONLIST::ResetButtons()
{
	if (!m_buttons.empty())
	{
		GSGUI_BUTTON_ITERATOR iter;
		for (iter = m_buttons.begin(); iter != m_buttons.end(); ++iter)
		{
			iter->active = false;
		}
	}
}

GSGUI_BUTTON GSGUI_BUTTONLIST::PlaceMenu(Vector2 v2Pos)
{
	GSGUI_BUTTON r;

	if (!m_video)
		return r;

	m_video->SetZBuffer(false);
	m_video->SetZWrite(false);

	Vector2 v2TextAdd(m_size/4.0f, 0.0f),
			   v2ButtonSize = Vector2(m_width, m_size);

	if (!m_buttons.empty())
	{
		bool newSingleSet = false;
		int focus=-1;
		int t=0;
		GSGUI_BUTTON_ITERATOR iter;
		for (iter = m_buttons.begin(); iter != m_buttons.end(); ++iter)
		{
			if (!iter->visible)
				continue;

			Vector2 v2ButtonPos = Vector2(v2Pos.x, v2Pos.y+((float)t*m_size));
			GS_DWORD top, bottom, text;

			iter->onFocus = MouseOver(v2ButtonPos, v2ButtonSize);

			if (iter->onFocus)
			{
				top = m_style.focused_top;
				bottom = m_style.focused_bottom;
				text = m_style.active_text;
				focus = t;

				if (m_input->GetLeftClickState() == GSKS_HIT)
				{
					if (m_unselect)
						iter->active = !(iter->active);
					else
						iter->active = true;

					if (m_singleSelect)
					{
						newSingleSet = true;
						m_single.text = iter->text;
					}
					r.text = iter->text;
					r.active = iter->active;
				}
			}
			else
			{
				top = m_style.inactive_top;
				bottom = m_style.inactive_bottom;
				text = m_style.inactive_text;
			}

			if (iter->active)
			{
				top = m_style.active_top;
				bottom = m_style.active_bottom;
				text = m_style.active_text;
			}

			m_video->DrawRectangle(v2ButtonPos, v2ButtonSize,
				top, top, bottom, bottom);
			str_type::stringstream ss;
			ss << iter->text << iter->extText;
			m_video->DrawBitmapText(
				v2ButtonPos + v2TextAdd, 
				ss.str().c_str(), GS_L("Verdana14_shadow.fnt"), text
			);
			t++;
		}

		if (newSingleSet)
		{
			for (iter = m_buttons.begin(); iter != m_buttons.end(); ++iter)
			{
				if (m_single.text == iter->text)
					continue;
				iter->active = false;
			}
			newSingleSet = false;
		}
		if (m_autoUnclick)
		{
			for (iter = m_buttons.begin(); iter != m_buttons.end(); ++iter)
			{
				iter->active = false;
			}
		}
	}

	if (MouseOver(v2Pos, Vector2(m_width, m_size*(float)m_buttons.size())))
	{
		m_mouseOver = true;
		DrawOutline(v2Pos, Vector2(m_width, m_size*(float)GetNumButtons()));
	}
	else
	{
		m_mouseOver = false;
	}

	return r;
}

void GSGUI_BUTTONLIST::AddButton(const str_type::string &text, const bool active, const str_type::string &extText)
{
	GSGUI_BUTTON button;
	button.active = active;
	button.text = text;
	button.extText = extText;
	m_buttons.push_back(button);
}

bool GSGUI_BUTTONLIST::GetButtonStatus(const str_type::string &text) const
{
	if (!m_buttons.empty())
	{
		GSGUI_BUTTON_CONST_ITERATOR iter;
		for (iter = m_buttons.begin(); iter != m_buttons.end(); ++iter)
		{
			if (text == iter->text)
			{
				return iter->active;
			}
		}
	}
	return false;
}

bool GSGUI_BUTTONLIST::ActivateButton(const str_type::string &text)
{
	if (!m_buttons.empty())
	{
		GSGUI_BUTTON_ITERATOR iter;
		for (iter = m_buttons.begin(); iter != m_buttons.end(); ++iter)
		{
			if (text == iter->text)
			{
				iter->active = true;
			}
		}
	}
	return true;
}


bool GSGUI_BUTTONLIST::DeactivateButton(const str_type::string &text)
{
	if (!m_buttons.empty())
	{
		GSGUI_BUTTON_ITERATOR iter;
		for (iter = m_buttons.begin(); iter != m_buttons.end(); ++iter)
		{
			if (text == iter->text)
			{
				iter->active = false;
			}
		}
	}
	return true;
}

void GSGUI_BUTTONLIST::DelButton(const str_type::string &text)
{
	if (!m_buttons.empty())
	{
		GSGUI_BUTTON_ITERATOR iter;
		for (iter = m_buttons.begin(); iter != m_buttons.end(); ++iter)
		{
			if (text == iter->text)
			{
				m_buttons.erase(iter);
				return;
			}
		}
	}
}

void GSGUI_BUTTONLIST::HideButton(const str_type::string &text, const bool hide)
{
	if (!m_buttons.empty())
	{
		GSGUI_BUTTON_ITERATOR iter;
		for (iter = m_buttons.begin(); iter != m_buttons.end(); ++iter)
		{
			if (text == iter->text)
			{
				iter->visible = !hide;
				return;
			}
		}
	}
}

GSGUI_BUTTON_LIST::const_iterator GSGUI_BUTTONLIST::Begin() const
{
	return m_buttons.begin();
}

GSGUI_BUTTON_LIST::const_iterator GSGUI_BUTTONLIST::End() const
{
	return m_buttons.end();
}

int GSGUI_BUTTONLIST::GetNumButtons() const
{
	int nButtons=0;
	GSGUI_BUTTON_CONST_ITERATOR iter;
	for (iter = m_buttons.begin(); iter != m_buttons.end(); ++iter)
	{
		if (iter->visible)
			nButtons++;
	}
	return nButtons;
}

GSGUI_BUTTON GSGUI_BUTTONLIST::GetFirstActiveButton() const
{
	GSGUI_BUTTON_CONST_ITERATOR iter;
	for (iter = m_buttons.begin(); iter != m_buttons.end(); ++iter)
	{
		if (iter->active)
			return *iter;
	}
	return GSGUI_BUTTON();
}

bool GSGUI_BUTTONLIST::IsMouseOver() const
{
	return m_mouseOver;
}

///////////////////////////////////////////////////////////////
// Int input
///////////////////////////////////////////////////////////////
GSGUI_INT_INPUT::GSGUI_INT_INPUT()
{
	//m_video = 0;
	//m_input = 0;
	m_size = 14.0f;
	m_width = 256.0f;
	m_active = false;
	m_strInput.NumbersOnly(true);
	m_text = GS_L("");
	m_nMaxChars = 8;
	m_min = 0;
	m_max = 0;
	m_clamp = false;
	m_active = false;
}

bool GSGUI_INT_INPUT::IsActive() const
{
	return m_active;
}

void GSGUI_INT_INPUT::SetActive(const bool b)
{
	m_active = b;
}

void GSGUI_INT_INPUT::SetupMenu(VideoPtr video, InputPtr input,
			   const float size, const float width, const int maxC, const bool active)
{
	m_video = video;
	m_input = input;
	m_size = size;
	m_width = width;
	m_active = active;
	m_nMaxChars = maxC;
	m_scrollAdd = 0;
}

void GSGUI_INT_INPUT::SetScrollAdd(const int add)
{
	m_scrollAdd = add;
}

GSGUI_BUTTON GSGUI_INT_INPUT::PlaceMenu(Vector2 v2Pos)
{
	GSGUI_BUTTON r;
	r.value.nValue = PlaceInput(v2Pos);
	return r;
}

int GSGUI_INT_INPUT::PlaceInput(Vector2 v2Pos)
{
	int r = 0;

	m_video->SetZBuffer(false);
	m_video->SetZWrite(false);

	Vector2 v2Size(m_width, m_size), v2TextAdd(m_size/4.0f, 0.0f);
	m_mouseOver = MouseOver(v2Pos, v2Size);
	Color top, bottom, text;

	if (m_active)
	{
		top = m_style.active_top;
		bottom = m_style.active_bottom;
		text = m_style.active_text;
		if (!m_mouseOver)
		{
			if (m_input->GetLeftClickState() == GSKS_HIT ||
				m_input->GetRightClickState() == GSKS_HIT)
				m_active = false;
		}
		if (m_input->GetKeyState(GSK_ENTER) == GSKS_HIT)
			m_active = false;
	}
	else
	{
		if (m_mouseOver)
		{
			if (m_input->GetLeftClickState() == GSKS_HIT)
				m_active = true;
			top = m_style.focused_top;
			bottom = m_style.focused_bottom;
			text = m_style.active_text;
		}
		else
		{
			top = m_style.inactive_top;
			bottom = m_style.inactive_bottom;
			text = m_style.inactive_text;
		}
		if (m_strInput.GetString() == GS_L(""))
			m_strInput.SetString(GS_L("0"));
	}

	m_video->DrawRectangle(v2Pos, Vector2(m_width, m_size),
							top, top,
							bottom, bottom);

	str_type::stringstream ss;
	ss << m_strInput.GetString();
	if (ss.str().length() > 0) //-V807
	{
		GS2D_SSCANF(ss.str().c_str(), GS_L("%i"), &r);
		if (m_active && m_input->GetWheelState() != 0.0f)
		{
			r += (int)m_input->GetWheelState()*m_scrollAdd;
			ss.str(GS_L(""));
			ss << r;
			m_strInput.SetString(ss.str());
		}

		GS2D_SSCANF(ss.str().c_str(), GS_L("%i"), &r);
		if (m_clamp)
		{
			if (r > m_max)
			{
				r = m_max;
				ss.str(GS_L(""));
				ss << r;
			} else
			if (r < m_min)
			{
				r = m_min;
				ss.str(GS_L(""));
				ss << r;
			}
			m_strInput.SetString(ss.str());
		}
	}
	if (m_active)
	{
		m_strInput.PlaceInput(v2Pos+v2TextAdd, GS_L("Verdana14_shadow.fnt"), m_nMaxChars, m_size, text, m_video, m_input);
	}
	else
	{
		m_strInput.Place(v2Pos+v2TextAdd, GS_L("Verdana14_shadow.fnt"), m_size, text, m_video);
	}

	if (m_active || m_mouseOver)
		DrawOutline(v2Pos, v2Size);

	return r;
}

bool GSGUI_INT_INPUT::IsMouseOver() const
{
	return m_mouseOver;
}

void GSGUI_INT_INPUT::SetClamp(const bool clamp, const int min, const int max)
{
	m_clamp = clamp;
	m_min = (min<max) ? min : max;
	m_max = (max>min) ? max : min;
}

void GSGUI_INT_INPUT::SetText(const str_type::string &text)
{
	m_text = text;
}

void GSGUI_INT_INPUT::SetConstant(const int n)
{
	str_type::stringstream ss;
	ss << n;
	m_strInput.SetString(ss.str());
}

///////////////////////////////////////////////////////////////
// Float input
///////////////////////////////////////////////////////////////
GSGUI_FLOAT_INPUT::GSGUI_FLOAT_INPUT()
{
	//m_video = 0;
	//m_input = 0;
	m_size = 14.0f;
	m_width = 256.0f;
	m_active = false;
	m_strInput.NumbersOnly(true);
	m_strInput.SetString(GS_L("0.0"));
	m_text.clear();
	m_nMaxChars = 8;
	m_min = 0.0f;
	m_max = 0.0f;
	m_clamp = false;
	m_scrollAdd = 0.1f;
	m_mouseOver = false;
	m_active = false;
}

void GSGUI_FLOAT_INPUT::SetScrollAdd(const float add)
{
	m_scrollAdd = add;
}

void GSGUI_FLOAT_INPUT::SetupMenu(VideoPtr video, InputPtr input,
			   const float size, const float width, const int max, const bool active)
{
	m_video = video;
	m_input = input;
	m_size = size;
	m_width = width;
	m_active = active;
	m_nMaxChars = max;
}

GSGUI_BUTTON GSGUI_FLOAT_INPUT::PlaceMenu(Vector2 v2Pos)
{
	GSGUI_BUTTON r;
	r.value.fValue = PlaceInput(v2Pos);
	return r;
}

bool GSGUI_FLOAT_INPUT::IsMouseOver() const
{
	return m_mouseOver;
}

bool GSGUI_FLOAT_INPUT::IsActive() const
{
	return m_active;
}

void GSGUI_FLOAT_INPUT::SetActive(const bool b)
{
	m_active = b;
}

float GSGUI_FLOAT_INPUT::PlaceInput(Vector2 v2Pos, Vector2 v2CommentPos, const float size)
{
	m_video->DrawBitmapText(
		v2CommentPos, 
		GetDescription().c_str(), GS_L("Verdana14_shadow.fnt"), gs2d::constant::WHITE
	);
	return PlaceInput(v2Pos);
}

float GSGUI_FLOAT_INPUT::PlaceInput(Vector2 v2Pos)
{
	m_lastValue = 0;

	m_video->SetZBuffer(false);
	m_video->SetZWrite(false);

	Vector2 v2Size(m_width, m_size), v2TextAdd(m_size/4.0f, 0.0f);
	m_mouseOver = MouseOver(v2Pos+Vector2(m_width, 0.0f), v2Size);
	Color top, bottom, text;

	m_video->DrawBitmapText(
		v2Pos + v2TextAdd, m_text.c_str(), 
		GS_L("Verdana14_shadow.fnt"), gs2d::constant::WHITE
	);

	if (m_active)
	{
		top = m_style.active_top;
		bottom = m_style.active_bottom;
		text = m_style.active_text;
		if (!m_mouseOver)
		{
			if (m_input->GetLeftClickState() == GSKS_HIT ||
				m_input->GetRightClickState() == GSKS_HIT)
				m_active = false;
		}
		if (m_input->GetKeyState(GSK_ENTER) == GSKS_HIT)
			m_active = false;
	}
	else
	{
		if (m_mouseOver)
		{
			if (m_input->GetLeftClickState() == GSKS_HIT)
				m_active = true;
			top = m_style.focused_top;
			bottom = m_style.focused_bottom;
			text = m_style.active_text;
		}
		else
		{
			top = m_style.inactive_top;
			bottom = m_style.inactive_bottom;
			text = m_style.inactive_text;
		}
		if (m_strInput.GetString() == GS_L(""))
			m_strInput.SetString(GS_L("0.0"));
	}

	m_video->DrawRectangle(v2Pos+Vector2(m_width, 0.0f), Vector2(m_width, m_size),
							top, top, bottom, bottom);

	str_type::stringstream ss;
	ss.str(m_strInput.GetString());

	// if there are no chars, go back to the min value
	//const unsigned int nLen = ss.str().length(); //-V807

	if (m_strInput.GetString().length() > 0)
	{
		GS2D_SSCANF(ss.str().c_str(), GS_L("%f"), &m_lastValue);

		if (m_active && m_input->GetWheelState() != 0.0f)
		{
			m_lastValue += m_input->GetWheelState()*m_scrollAdd;
			ss.str(GS_L(""));
			ss << m_lastValue;
			m_strInput.SetString(ss.str());
		}

		GS2D_SSCANF(ss.str().c_str(), GS_L("%f"), &m_lastValue);
		if (m_clamp)
		{
			if (m_lastValue > m_max)
			{
				m_lastValue = m_max;
				ss.str(GS_L(""));
				ss << m_lastValue;
			} else
			if (m_lastValue < m_min)
			{
				m_lastValue = m_min;
				ss.str(GS_L(""));
				ss << m_lastValue;
			}
			m_strInput.SetString(ss.str());
		}
	}

	if (m_active)
	{
		m_strInput.PlaceInput(v2Pos+v2TextAdd+Vector2(m_width, 0.0f), GS_L("Verdana14_shadow.fnt"), m_nMaxChars, m_size, text, m_video, m_input);
	}
	else
	{
		m_strInput.Place(v2Pos+v2TextAdd+Vector2(m_width, 0.0f), GS_L("Verdana14_shadow.fnt"), m_size, text, m_video);
	}

	if (m_active || m_mouseOver)
		DrawOutline(v2Pos, v2Size+Vector2(m_width, 0.0f));

	return m_lastValue;
}

str_type::string GSGUI_FLOAT_INPUT::GetDescription()
{
	if (!IsMouseOver())
		return GS_L("");
	return m_desc;
}

float GSGUI_FLOAT_INPUT::GetLastValue() const
{
	return m_lastValue;
}

void GSGUI_FLOAT_INPUT::SetText(const str_type::string &text)
{
	m_text = text;
}

void GSGUI_FLOAT_INPUT::SetClamp(const bool clamp, const float min, const float max)
{
	m_clamp = clamp;
	m_min = (min<max) ? min : max;
	m_max = (max>min) ? max : min;
}

void GSGUI_FLOAT_INPUT::SetConstant(const float f)
{
	str_type::stringstream ss;
	ss << f;
	m_strInput.SetString(ss.str());
	m_lastValue = f;
	//const unsigned int n = ss.str().length();
}

///////////////////////////////////////////////////////////////
// String line input
///////////////////////////////////////////////////////////////
GSGUI_STRING_LINE_INPUT::GSGUI_STRING_LINE_INPUT()
{
	m_strInput.SetString(GS_L(""));
	//m_video = 0;
	//m_input = 0;
	m_size = 14.0f;
	m_width = 256.0f;
	m_active = false;
	m_mouseOver = false;
	m_text = GS_L("");
	m_nMaxChars = 8;
}

void GSGUI_STRING_LINE_INPUT::SetValue(const str_type::string &str)
{
	m_strInput.SetString(str);
}

str_type::string GSGUI_STRING_LINE_INPUT::GetValue() const
{
	return m_strInput.GetString();
}

void GSGUI_STRING_LINE_INPUT::SetupMenu(VideoPtr video, InputPtr input,
										const float size, const float width, const int maxC, const bool active,
										const str_type::string& sValue)
{
	m_video = video;
	m_input = input;
	m_size = size;
	m_width = width;
	m_active = active;
	m_nMaxChars = maxC;
	m_strInput.SetString(sValue);
}

str_type::string GSGUI_STRING_LINE_INPUT::PlaceInput(Vector2 v2Pos)
{
	m_video->SetZBuffer(false);
	m_video->SetZWrite(false);

	Vector2 v2Size(m_width, m_size), v2TextAdd(m_size/4.0f, 0.0f);
	m_mouseOver = MouseOver(v2Pos, v2Size);
	Color top, bottom, text;

	// const Vector2i scissor = m_video->GetTextRect();
	// m_video->SetTextRect(Vector2i(int(GetWidth()-v2TextAdd.x), int(GetSize()-v2TextAdd.y)));

	if (m_active)
	{
		top = m_style.active_top;
		bottom = m_style.active_bottom;
		text = m_style.active_text;
		if (!m_mouseOver)
		{
			if (m_input->GetLeftClickState() == GSKS_HIT ||
				m_input->GetRightClickState() == GSKS_HIT)
				m_active = false;
		}
		if (m_input->GetKeyState(GSK_ENTER) == GSKS_HIT)
			m_active = false;
	}
	else
	{
		if (m_mouseOver)
		{
			if (m_input->GetLeftClickState() == GSKS_HIT)
				m_active = true;
			top = m_style.focused_top;
			bottom = m_style.focused_bottom;
			text = m_style.active_text;
		}
		else
		{
			top = m_style.inactive_top;
			bottom = m_style.inactive_bottom;
			text = m_style.inactive_text;
		}
		if (m_strInput.GetString() == GS_L(""))
			m_strInput.SetString(GS_L("0"));
	}

	m_video->DrawRectangle(v2Pos, Vector2(m_width, m_size),
							top, top,
							bottom, bottom);

	str_type::stringstream ss;
	ss << m_strInput.GetString();

	if (m_active)
	{
		m_strInput.PlaceInput(v2Pos+v2TextAdd, GS_L("Verdana14_shadow.fnt"), m_nMaxChars, m_size, text, m_video, m_input);
	}
	else
	{
		m_strInput.Place(v2Pos+v2TextAdd, GS_L("Verdana14_shadow.fnt"), m_size, text, m_video);
	}
	if (m_active || m_mouseOver)
		DrawOutline(v2Pos, v2Size);

	// m_video->SetTextRect(scissor);

	return m_strInput.GetString();
}

bool GSGUI_STRING_LINE_INPUT::IsMouseOver() const
{
	return m_mouseOver;
}

bool GSGUI_STRING_LINE_INPUT::IsActive() const
{
	return m_active;
}

void GSGUI_STRING_LINE_INPUT::SetActive(const bool b)
{
	m_active = b;
}

void GSGUI_STRING_LINE_INPUT::SetText(const str_type::string &text)
{
    m_text = text;
}

GSGUI_BUTTON GSGUI_STRING_LINE_INPUT::PlaceMenu(Vector2 v2Pos)
{
	GSGUI_BUTTON r;
	r.text = PlaceInput(v2Pos);
	return r;
}
