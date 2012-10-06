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

#ifndef GUISPACE_GAMESPACE_34982095803257023
#define GUISPACE_GAMESPACE_34982095803257023

#include <gs2d.h>
#include <Audio.h>
#include <Input.h>
#include <gs2dplayer.h>
using namespace gs2d::math;
using namespace gs2d;

#include <string>
#include <sstream>
#include <list>
#include <boost/shared_ptr.hpp>

///////////////////////////////////////////////////////////////
// string input class
///////////////////////////////////////////////////////////////
class GSGUI_STRING_INPUT
{
public:
	GSGUI_STRING_INPUT();
	void PlaceInput(const Vector2& pos, const std::wstring& font, const unsigned int nMaxChars,
					const float size, const Color& dwColor, VideoPtr video,
					InputPtr input);
	void Place(const Vector2& pos, const std::wstring& font,
				const float size, const Color& dwColor, VideoPtr video);
	std::wstring GetString() const;
	void SetString(const std::wstring &inS);
	void NumbersOnly(const bool b);
	void SendCursorToEnd();

private:
	unsigned int blinkTime;
	unsigned int lastBlink;
	unsigned int showingCarret;
	unsigned int cursor;
	bool numbersOnly;
	std::wstring ss;
};

///////////////////////////////////////////////////////////////
// style class
///////////////////////////////////////////////////////////////
struct GSGUI_STYLE
{
	GSGUI_STYLE();
	Color outline;

	Color inactive_top;
	Color inactive_bottom;
	Color active_top;
	Color active_bottom;
	Color focused_top;
	Color focused_bottom;

	Color active_text;
	Color inactive_text;
	Color text_shadow;
};

///////////////////////////////////////////////////////////////
// default button
///////////////////////////////////////////////////////////////
struct GSGUI_BUTTON
{
	GSGUI_BUTTON();
	bool onFocus;
	bool active;
	bool visible;
	std::wstring text;
	std::wstring extText;
	union GSGUI_BUTTONVALUE
	{
		int nValue;
		float fValue;
	} value;
};

typedef std::list<GSGUI_BUTTON> GSGUI_BUTTON_LIST;
typedef std::list<GSGUI_BUTTON>::iterator GSGUI_BUTTON_ITERATOR;
typedef std::list<GSGUI_BUTTON>::const_iterator GSGUI_BUTTON_CONST_ITERATOR;

///////////////////////////////////////////////////////////////
// GUI base
///////////////////////////////////////////////////////////////
class GS_GUI_ACTIVABLE_OBJECT
{
public:
	virtual bool IsActive() const = 0;
	virtual void SetActive(const bool b) = 0;
};

class GS_GUI
{
public:
	GS_GUI();
	virtual GSGUI_BUTTON PlaceMenu(Vector2 v2Pos) = 0;
	virtual std::wstring GetDescription();
	virtual void SetDescription(const std::wstring &desc);
	const GSGUI_STYLE *GetGUIStyle() const;
	float GetWidth() const;
	float GetSize() const;
	float GetCurrentHeight() const;
	virtual bool IsMouseOver() const = 0;

protected:
	void Reset();

	VideoPtr m_video;
	InputPtr m_input;
	GSGUI_STYLE m_style;
	float m_size;
	float m_width;
	std::wstring m_desc;

	virtual bool MouseOver(Vector2 v2Pos, Vector2 v2Size);
	virtual void DrawOutline(Vector2 v2Pos, Vector2 v2Size);
};

///////////////////////////////////////////////////////////////
// Swap image button
///////////////////////////////////////////////////////////////

/// A button that swaps it's image on click
class GSGUI_SWAPBUTTON : public GS_GUI
{
public:
	struct GSGUI_SWAPSPRITE
	{
		SpritePtr sprite;
		std::wstring fileName;
	};
	typedef boost::shared_ptr<GSGUI_SWAPSPRITE> GSGUI_SWAPSPRITE_PTR;
	typedef boost::shared_ptr<const GSGUI_SWAPSPRITE> GSGUI_SWAPSPRITE_CONST_PTR;
	typedef std::list<GSGUI_SWAPSPRITE_PTR> GSGUI_SWAPSPRITE_PTR_LIST;

	void SetupButtons(VideoPtr video, InputPtr input);
	GSGUI_SWAPSPRITE_CONST_PTR GetCurrentButton() const;
	bool AddButton(const std::wstring &fileName);

	GSGUI_BUTTON PlaceMenu(Vector2 v2Pos);
	bool IsMouseOver() const;

	bool SetButton(const std::wstring &fileName);

private:
	GSGUI_SWAPSPRITE_PTR_LIST::iterator Find(const std::wstring &fileName);

	GSGUI_SWAPSPRITE_PTR m_current;
	GSGUI_SWAPSPRITE_PTR_LIST m_sprites;
	bool m_mouseOver;
};

///////////////////////////////////////////////////////////////
// Dropdown menu
///////////////////////////////////////////////////////////////
class GSGUI_DROPDOWN : public GS_GUI, public GS_GUI_ACTIVABLE_OBJECT
{
public:
	GSGUI_DROPDOWN();
	~GSGUI_DROPDOWN();
	void Destroy();
	void SetupMenu(VideoPtr video, InputPtr input, const std::wstring &mainText,
				   const float size, const float width, const bool onclick);
	GSGUI_BUTTON PlaceMenu(Vector2 v2Pos);
	void AddButton(const std::wstring &text);
	void DelButton(const std::wstring &text);
	int GetNumButtons() const;
	bool IsActive() const;
	void SetActive(const bool b);
	bool IsMouseOver() const;
	float GetCurrentHeight() const;

private:
	GSGUI_BUTTON_LIST m_buttons;
	bool m_onClick;
	bool m_mouseOver;
	bool m_active;
	std::wstring m_mainText;
};

///////////////////////////////////////////////////////////////
// Custom button list
///////////////////////////////////////////////////////////////
class GSGUI_BUTTONLIST : public GS_GUI
{
public:
	GSGUI_BUTTONLIST();
	~GSGUI_BUTTONLIST();
	void Destroy();
	void Clear();
	void SetupMenu(VideoPtr video, InputPtr input,
				   const float size, const float width, const bool single, const bool unselect,
				   const bool autounclick);
	void ResetButtons();
	GSGUI_BUTTON PlaceMenu(Vector2 v2Pos);
	void AddButton(const std::wstring &text, const bool active = false, const std::wstring &extText = L"");
	bool GetButtonStatus(const std::wstring &text) const;
	bool ActivateButton(const std::wstring &text);
	bool DeactivateButton(const std::wstring &text);
	void DelButton(const std::wstring &text);
	int GetNumButtons() const;
	bool IsMouseOver() const;
	void HideButton(const std::wstring &text, const bool hide);
	GSGUI_BUTTON_LIST::const_iterator Begin() const;
	GSGUI_BUTTON_LIST::const_iterator End() const;
	GSGUI_BUTTON GetFirstActiveButton() const;

private:
	GSGUI_BUTTON_LIST m_buttons;

	bool m_mouseOver;
	bool m_singleSelect;
	bool m_unselect;
	bool m_autoUnclick;
	GSGUI_BUTTON m_single;
};

///////////////////////////////////////////////////////////////
// Integer input
///////////////////////////////////////////////////////////////
class GSGUI_INT_INPUT : public GS_GUI, public GS_GUI_ACTIVABLE_OBJECT
{
public:
	GSGUI_INT_INPUT();
	void SetupMenu(VideoPtr video, InputPtr input,
				   const float size, const float width, const int maxC, const bool active);
	int PlaceInput(Vector2 v2Pos);
	GSGUI_BUTTON PlaceMenu(Vector2 v2Pos);
	void SetClamp(const bool clamp, const int min, const int max);
	void SetText(const std::wstring &text);
	void SetConstant(const int n);
	void SetScrollAdd(const int add);
	bool IsMouseOver() const;
	bool IsActive() const;
	void SetActive(const bool b);

private:
	bool m_mouseOver;
	int m_nMaxChars;
	int m_min, m_max;
	bool m_clamp;
	bool m_active;
	std::wstring m_text;
	GSGUI_STRING_INPUT m_strInput;
	int m_scrollAdd;
};

///////////////////////////////////////////////////////////////
// float input
///////////////////////////////////////////////////////////////
class GSGUI_FLOAT_INPUT : public GS_GUI, public GS_GUI_ACTIVABLE_OBJECT
{
public:
	GSGUI_FLOAT_INPUT();
	void SetScrollAdd(const float add);
	void SetupMenu(VideoPtr video, InputPtr input,
				   const float size, const float width, const int max, const bool active);
	GSGUI_BUTTON PlaceMenu(Vector2 v2Pos);
	bool IsMouseOver() const;
	float PlaceInput(Vector2 v2Pos, Vector2 v2CommentPos, const float size);
	float PlaceInput(Vector2 v2Pos);
	std::wstring GetDescription();
	void SetText(const std::wstring &text);
	void SetClamp(const bool clamp, const float min, const float max);
	void SetConstant(const float f);
	float GetLastValue() const;
	bool IsActive() const;
	void SetActive(const bool b);

private:
	bool m_mouseOver;
	int m_nMaxChars;
	float m_min, m_max, m_scrollAdd;
	float m_lastValue;
	bool m_clamp;
	bool m_active;
	GSGUI_STRING_INPUT m_strInput;
	std::wstring m_text;
};

///////////////////////////////////////////////////////////////
// string line input
///////////////////////////////////////////////////////////////
class GSGUI_STRING_LINE_INPUT : public GS_GUI, public GS_GUI_ACTIVABLE_OBJECT
{
public:
	GSGUI_STRING_LINE_INPUT();
	void SetupMenu(VideoPtr video, InputPtr input,
				   const float size, const float width, const int maxC, const bool active,
				   const std::wstring& sValue = L"");
	GSGUI_BUTTON PlaceMenu(Vector2 v2Pos);
	std::wstring PlaceInput(Vector2 v2Pos);
	void SetText(const std::wstring &text);
	bool IsMouseOver() const;
	bool IsActive() const;
	void SetActive(const bool b);
	void SetValue(const std::wstring &str);
	std::wstring GetValue() const;

private:
	bool m_mouseOver;
	bool m_active;
	int m_nMaxChars;
	std::wstring m_text;
	GSGUI_STRING_INPUT m_strInput;
};
#endif
