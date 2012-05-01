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

string getTimeString(const uint time)
{
	const uint secs = (time/1000)%60;
	string seconds;
	if (secs < 10)
		seconds = "0"+secs;
	else
		seconds = ""+secs;
	return "" + (time/1000)/60 + ":" + seconds;
}

class Timer
{
	Timer()
	{
		startTime = 0;
	}
	
	void start()
	{
		startTime = GetTime();
	}
	
	uint getElapsedTime()
	{
		return GetTime()-startTime;
	}
	
	void showTimer(const vector2 pos, const float size, const uint8 a,
				   const uint8 r, const uint8 g, const uint8 b)
	{
		const uint elapsed = getElapsedTime();
		string time = getTimeString(elapsed);
		shadowText(pos, time, "Arial Narrow", size, a, r, g, b);
	}
	
	private uint startTime;
}
