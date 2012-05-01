/*
PORTUGUÊS
---------

Este arquivo é parte do Penumbra.

Penumbra é software livre; você pode redistribuí-lo e/ou
modificá-lo sob os termos da Licença Pública Menos Geral (LGPL)
GNU, conforme publicada pela Free Software Foundation; tanto a
versão 3 da Licença como (a seu critério) qualquer versão mais
nova.

Penumbra é distribuído na expectativa de ser útil, mas
SEM QUALQUER GARANTIA; sem mesmo a garantia implícita de
COMERCIALIZAÇÃO ou de ADEQUAÇÃO A QUALQUER PROPÓSITO EM
PARTICULAR. Consulte a Licença Pública Geral Menor (LGPL) GNU
para obter mais detalhes.

Você deve ter recebido uma cópia da Licença Pública Menos Geral
GNU junto com este programa; se não, veja em
<http://www.gnu.org/licenses/>.

ENGLISH
-------

This file is part of Penumbra.

Penumbra is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

Penumbra is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with Ethanon Engine. If not, see
<http://www.gnu.org/licenses/>.
*/

class Message
{
	Message()
	{
		time = 0;
		message = "";
		alpha = 0;
		origin = vector2(0,0);
	}
	
	uint time;
	string message;
	uint8 alpha;
	vector2 origin;
}

class MessageManager
{
	MessageManager()
	{
		maxMessages = 10;
		maxDamageMessages = 15;
		messageTime = 4000;
		damage.resize(maxDamageMessages);
		damageIndex = 0;
	}

	void addMessage(const int damageValue, const vector2 origin)
	{
		if (damageIndex >= maxDamageMessages)
			damageIndex = 0;
		
		damage[damageIndex].time = GetTime();
		damage[damageIndex].message = ""+damageValue;
		damage[damageIndex].alpha = 255;
		damage[damageIndex].origin = origin+vector2(-10,-32);
		damageIndex++;
	}

	void addMessage(const string msg)
	{
		if (!SampleExists("soundfx/help.mp3"))
		{
			LoadSoundEffect("soundfx/help.mp3");
		}
		if (/*!IsSamplePlaying("soundfx/help.mp3")*/
			lastMessage != msg)
		{
			PlaySample("soundfx/help.mp3");
		}
		
		uint t;
		bool messageExists = false;
		for (t=0; t<maxMessages; t++)
		{
			const string key = getKey(t);
			Message @msgHandle;
			if (dict.get(key, @msgHandle))
			{
				if (msg == msgHandle.message)
				{
					messageExists = true;
					return;
				}
			}
		}

		Message newMsg;
		newMsg.time = GetTime();
		newMsg.message = msg;

		if (!messageExists)
		{
			for (t=0; t<maxMessages; t++)
			{
				const string key = getKey(t);
				if (!dict.exists(key))
				{
					dict.set(key, @newMsg);
					return;
				}
			}
		}
	}

	void showMessages(const vector2 pos, const string font, const float size,
					  uint8 r, uint8 g, uint8 b, CameraManager @camera)
	{
		processMessages();
		Message @latest = null;
		const float smallerTextSize = size/2;
		for (uint t=0; t<maxMessages; t++)
		{
			const string key = getKey(t);
			Message @msg;
			if (dict.get(key, @msg))
			{
				if (latest is null)
				{
					@latest = @msg;
					lastMessage = msg.message;
				}
				if (latest.time <= msg.time)
				{
					lastMessage = msg.message;
					@latest = @msg;
				}
				const vector2 msgPos = pos+vector2(0,float(t)*size);
				shadowText(msgPos, msg.message, font, size, msg.alpha,r,g,b);
			}
		}
		if (latest !is null)
		{
			vector2 charPos = camera.getMainCharPos(0)-GetCameraPos();
			charPos.x -= 64;
			charPos.y += 32;
			shadowText(charPos, latest.message, font, smallerTextSize, latest.alpha/2,r,g,b);
		}
		else
		{
			lastMessage = "";
		}
		
		const float damageTextSize = size/2;
		for (uint t=0; t<maxDamageMessages; t++)
		{
			const uint elapsed = GetTime()-damage[t].time;
			if (elapsed < messageTime/3)
			{
				shadowText(damage[t].origin-GetCameraPos(), damage[t].message, font, damageTextSize, damage[t].alpha,r,g,b);
			}
		}
	}

	private void processMessages()
	{
		for (uint t=0; t<maxMessages; t++)
		{
			const string key = getKey(t);
			Message @msg;
			if (dict.get(key, @msg))
			{
				const uint elapsed = (GetTime()-msg.time);
				if (elapsed < messageTime)
				{
					msg.alpha = 255-((elapsed*255)/messageTime);
				}
				else
				{
					dict.delete(key);
				}
			}
		}
		
		for (uint t=0; t<maxDamageMessages; t++)
		{
			const uint elapsed = GetTime()-damage[t].time;
			if (elapsed < messageTime/3)
			{
				damage[t].alpha = 255-((elapsed*255)/(messageTime/3));
				damage[t].origin.y -= UnitsPerSecond(15);
			}
		}
	}
	
	string getKey(const uint n)
	{
		return ("k" + n);
	}
	
	private dictionary dict;
	private uint maxMessages;
	private uint messageTime;
	private string lastMessage;
	private Message[] damage;
	private uint damageIndex;
	private uint maxDamageMessages;
}