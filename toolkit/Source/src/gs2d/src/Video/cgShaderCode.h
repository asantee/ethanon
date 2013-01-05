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

#ifndef GS2D_DEFAULT_SHADER_H_
#define GS2D_DEFAULT_SHADER_H_

namespace gs2d {
namespace gs2dglobal {

// the default vertex shader
const char defaultVSCode[] = 
"uniform float4x4 viewMatrix;\n" \
"uniform float4x4 rotationMatrix;\n" \
"uniform float2 screenSize;\n" \
"uniform float2 size;\n" \
"uniform float2 entityPos;\n" \
"uniform float2 center;\n" \
"uniform float2 bitmapSize;\n" \
"uniform float2 rectPos;\n" \
"uniform float2 rectSize;\n" \
"uniform float2 scroll;\n" \
"uniform float2 multiply;\n" \
"uniform float4 color0;\n" \
"uniform float4 color1;\n" \
"uniform float4 color2;\n" \
"uniform float4 color3;\n" \
"uniform float2 flipAdd;\n" \
"uniform float2 flipMul;\n" \
"uniform float2 cameraPos;\n" \
"uniform float depth;\n" \
"float4 transformSprite(float3 position)\n" \
"{\n" \
"	float4 newPos = float4(position, 1) * float4(flipMul,1,1) + float4(flipAdd,0,0);\n" \
"	newPos = newPos * float4(size,1,1) - float4(center, 0, 0);\n" \
"	newPos = mul(rotationMatrix, newPos);\n" \
"	newPos += float4(entityPos,0,0)-float4(screenSize/2,0,0)-float4(cameraPos,0,0);\n" \
"	newPos *= float4(1,-1,1,1);\n" \
"	return mul(viewMatrix, newPos);\n" \
"}\n" \
"float4 transformRectangle(float3 position)\n" \
"{\n" \
"	float4 newPos = float4(position, 1) * float4(size,1,1) - float4(center, 0, 0);\n" \
"	newPos = mul(rotationMatrix, newPos);\n" \
"	newPos += float4(entityPos,0,0)-float4(screenSize/2,0,0);\n" \
"	newPos *= float4(1,-1,1,1);\n" \
"	return mul(viewMatrix, newPos);\n" \
"}\n" \
"float2 transformCoord(float2 texCoord)\n" \
"{\n" \
"	float2 newCoord = texCoord * (rectSize/bitmapSize);\n" \
"	newCoord += (rectPos/bitmapSize);\n" \
"	return (newCoord+(scroll/bitmapSize))*multiply;\n" \
"}\n" \
"float4 getVertexColor(float3 position)\n" \
"{\n" \
"	float4 vertex0 = color0 * (1-position.x) * (1-position.y);\n" \
"	float4 vertex1 = color1 * (position.x) * (1-position.y);\n" \
"	float4 vertex2 = color2 * (1-position.x) * (position.y);\n" \
"	float4 vertex3 = color3 * (position.x) * (position.y);\n" \
"	return vertex0 + vertex1 + vertex2 + vertex3;\n" \
"}\n" \
"void sprite(float3 position : POSITION,\n" \
"			float2 texCoord : TEXCOORD0,\n" \
"			out float4 oPosition : POSITION,\n" \
"			out float4 oColor    : COLOR0,\n" \
"			out float2 oTexCoord0 : TEXCOORD0,\n" \
"			out float2 oTexCoord1 : TEXCOORD1)\n" \
"{\n" \
"	float4 spritePos = transformSprite(position);\n" \
"	spritePos.z = depth;\n" \
"	oPosition = spritePos;\n" \
"	const float2 finalTexCoord = transformCoord(texCoord);\n" \
"	oTexCoord0 = oTexCoord1 = finalTexCoord;\n" \
"	oColor = getVertexColor(position);\n" \
"}\n" \
"void rectangle(float3 position : POSITION,\n" \
"			   out float4 oPosition : POSITION,\n" \
"			   out float4 oColor : COLOR0)\n" \
"{\n" \
"	oPosition = transformRectangle(position);\n" \
"	oColor = getVertexColor(position);\n" \
"}";


const char fastSimpleVSCode[] = 
"uniform float4x4 viewMatrix;\n" \
"uniform float2 screenSize;\n" \
"uniform float2 size;\n" \
"uniform float2 entityPos;\n" \
"uniform float2 bitmapSize;\n" \
"uniform float2 rectPos;\n" \
"uniform float2 rectSize;\n" \
"uniform float4 color0;\n" \
"float4 transformFast(float3 position)\n" \
"{\n" \
"	float4 newPos = float4(position, 1) * float4(size,1,1);\n" \
"	newPos += float4(entityPos,0,0)-float4(screenSize/2,0,0);\n" \
"	newPos *= float4(1,-1,1,1);\n" \
"	return mul(viewMatrix, newPos);\n" \
"}\n" \
"float2 transformFastCoord(float2 texCoord)\n" \
"{\n" \
"	float2 newCoord = texCoord * (rectSize/bitmapSize);\n" \
"	newCoord += (rectPos/bitmapSize);\n" \
"	return newCoord;\n" \
"}\n" \
"void fast(float3 position : POSITION, float2 texCoord : TEXCOORD0,\n" \
"		   out float4 oPosition : POSITION, out float2 oTexCoord : TEXCOORD0,\n" \
"		   out float4 oColor : COLOR0)\n" \
"{\n" \
"	oPosition = transformFast(position);\n" \
"	oTexCoord = transformFastCoord(texCoord);\n" \
"	oColor = color0;\n" \
"}";

} // namespace gs2dglobal
} // namespace gs2d

#endif
