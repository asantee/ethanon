/*-----------------------------------------------------------------------

 Ethanon Engine (C) Copyright 2009-2010 André Santee
 http://www.asantee.net/gamespace/ethanon/

  ENGLISH
  -------

    This file is part of Ethanon Engine.

    Ethanon Engine is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    Ethanon Engine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Ethanon Engine. If not, see
    <http://www.gnu.org/licenses/>.

  PORTUGUÊS
  ---------

    Este arquivo é parte da Ethanon Engine.

    Ethanon Engine é software livre; você pode redistribuí-lo e/ou
    modificá-lo sob os termos da Licença Pública Menos Geral (LGPL)
    GNU, conforme publicada pela Free Software Foundation; tanto a
    versão 2 da Licença como (a seu critério) qualquer versão mais
    nova.

    Ethanon Engine é distribuído na expectativa de ser útil, mas
    SEM QUALQUER GARANTIA; sem mesmo a garantia implícita de
    COMERCIALIZAÇÃO ou de ADEQUAÇÃO A QUALQUER PROPÓSITO EM
    PARTICULAR. Consulte a Licença Pública Geral Menor (LGPL) GNU
    para obter mais detalhes.

    Você deve ter recebido uma cópia da Licença Pública Menos Geral
    GNU junto com este programa; se não, veja em
    <http://www.gnu.org/licenses/>.

-----------------------------------------------------------------------*/

// Creates a string from a vector3
string Vector3ToString(const vector3 v3)
{
	return "(" + v3.x + ", " + v3.y + ", " + v3.z + ")";
}

// Creates a string from a vector2
string Vector2ToString(const vector2 v2)
{
	return "(" + v2.x + ", " + v2.y + ")";
}

// Converts a pixel format assignment to a stringInput
string FormatToString(const PIXEL_FORMAT format)
{
	if (format == PF32BIT)
		return "32";
	if (format == PF16BIT)
		return "16";
	return "unknown";
}


void DrawText(const vector2 pos, const string outputString, const string sFont, const float fSize, const uint dwColor)
{
	//DrawText(pos, outputString, sFont + fSize + ".fnt", dwColor);
	// TODO: make all needed .fnt files and uncomment the line above
}

/* 
 * stringInput class:
 * Places an input area on screen where the user can type texts
 */
class stringInput
{
	stringInput()
	{
		blinkTime = 300;
		lastBlink = 0;
		showingCarret = 1;
	}
	void PlaceInput(const string sText, const vector2 pos, const string sFont,
					const float fSize, const uint dwColor)
	{
		const uint time = GetTime();
		if ((time-lastBlink) > blinkTime)
		{
			showingCarret = showingCarret==0 ? 1 : 0;
			lastBlink = GetTime();
		}
	
		ETHInput @input = GetInputHandle();
		
		string lastInput = input.GetLastCharInput();
		if (lastInput != "")
		{
			ss += lastInput;
		}
		
		if (input.GetKeyState(K_BACKSPACE) == KS_HIT || input.GetKeyState(K_LEFT) == KS_HIT)
		{
			const uint nLen = ss.length();
			if (nLen > 0)
				ss.resize(nLen-1);
		}
		
		string outputString = sText + ": " + ss;
		if (showingCarret==1)
			outputString += "|";
		DrawText(pos, outputString, sFont, fSize, dwColor);
	}
	
	string GetString()
	{
		return ss;
	}
	
	private uint blinkTime;
	private uint lastBlink;
	private uint showingCarret;
	private string ss;
}

/* 
 * frameTimer class:
 * This object helps handling keyframe animation
 */
class frameTimer
{
	frameTimer()
	{
		m_currentFrame = m_currentFirst = m_currentLast = 0;
		m_lastTime = 0;
	}

	uint Get()
	{
		return m_currentFrame;
	}

	uint Set(const uint nFirst, const uint nLast, const uint nStride)
	{
		if (nFirst != m_currentFirst or nLast != m_currentLast)
		{
			m_currentFrame = nFirst;
			m_currentFirst = nFirst;
			m_currentLast  = nLast;
			m_lastTime = GetTime();
			return m_currentFrame;
		}

		if (GetTime()-m_lastTime > nStride)
		{
			m_currentFrame++;
			if (m_currentFrame > nLast)
				m_currentFrame = nFirst;
			m_lastTime = GetTime();
		}

		return m_currentFrame;
	}

	private uint m_lastTime;
	private uint m_currentFirst;
	private uint m_currentLast;
	private uint m_currentFrame;
}
