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

void ETHCallback_clouds(ETHEntity @thisEntity)
{
	const uint lightningTime = 1600;
	const float[] peaks = {
		0.7f,
		0.8f,
		0.9f,
		1.0f,
		1.0f,
		0.8f,
		0.6f,
		0.4f,
		0.8f,
		1.0f,
		0.9f,
		0.8f,
		0.6f,
		0.3f,
		0.1f
	};

	positionEnvironmentElements(thisEntity);
	
	if (thisEntity.CheckCustomData("ambientR") == DT_NODATA)
	{
		thisEntity.AddFloatData("ambientR", GetAmbientLight().x);
		thisEntity.AddFloatData("ambientG", GetAmbientLight().y);
		thisEntity.AddFloatData("ambientB", GetAmbientLight().z);
	}
	if (thisEntity.CheckCustomData("lastLightning") == DT_NODATA)
	{
		thisEntity.AddUIntData("lastLightning", GetTime());
	}
	if (thisEntity.CheckCustomData("nextLightning") == DT_NODATA)
	{
		thisEntity.AddUIntData("nextLightning", 10000);
	}

	const vector3 ambient(
		thisEntity.GetFloatData("ambientR"),
		thisEntity.GetFloatData("ambientG"),
		thisEntity.GetFloatData("ambientB"));

	const uint elapsed = GetTime()-thisEntity.GetUIntData("lastLightning");
	if (elapsed >= thisEntity.GetUIntData("nextLightning"))
	{
		PlaySample("soundfx/thunder.mp3");
		thisEntity.AddUIntData("lastLightning", GetTime());
		thisEntity.AddUIntData("nextLightning", 6000+rand(6000));
	}
	else if (elapsed < lightningTime)
	{
		uint index = uint((float(elapsed)/float(lightningTime))*float(peaks.length()));
		if (index >= peaks.length())
			index = peaks.length()-1;
		const float peak = peaks[index];
		uint8 color = uint8(peak*255.0f);
		SetBackgroundColor(ARGB(255, color, color, color));
		vector3 peakColor(1-peak, 1-peak, 1-peak);
		peakColor.x *= ambient.x;
		peakColor.y *= ambient.y;
		peakColor.z *= ambient.z;
		SetAmbientLight(peakColor);
	}
	else
	{
		if (thisEntity.CheckCustomData("bgColor") != DT_NODATA)
			SetBackgroundColor(thisEntity.GetUIntData("bgColor"));
		SetAmbientLight(ambient);
	}
}

void ETHCallback_fog(ETHEntity @thisEntity)
{
	positionEnvironmentElements(thisEntity);
	if (thisEntity.CheckCustomData("bgColor") != DT_NODATA)
		SetBackgroundColor(thisEntity.GetUIntData("bgColor"));
}

void positionEnvironmentElements(ETHEntity @thisEntity)
{
	const vector2 cam = GetCameraPos();
	thisEntity.SetPosition(vector3(cam.x+64, cam.y+350, 0));

	if (thisEntity.CheckCustomData("bgImage") != DT_NODATA)
	{
		SetBackgroundImage("entities/" + thisEntity.GetStringData("bgImage"));
		if (thisEntity.GetStringData("bgImage") == "planets.png")
		{
			SetBackgroundAlphaAdd();
			vector2 screenSize(GetScreenSize());
			const vector2 bgMin(screenSize.x*0.6f, 100);
			const vector2 bgMax(bgMin+vector2(329,329));
			PositionBackgroundImage(bgMin, bgMax);
		}
		else
		{
			PositionBackgroundImage(vector2(0,0), GetScreenSize());
		}
	}
}

void ETHCallback_dawn(ETHEntity @thisEntity)
{
	if (thisEntity.CheckCustomData("bgColor") != DT_NODATA)
		SetBackgroundColor(thisEntity.GetUIntData("bgColor"));

	if (thisEntity.CheckCustomData("bgImage") != DT_NODATA)
	{
		SetBackgroundImage("entities/" + thisEntity.GetStringData("bgImage"));
		PositionBackgroundImage(vector2(0,0), GetScreenSize());
	}
}
