/* This file has been generated with the ShaderToCHeader tool. Do not edit! */


#ifndef ETH_DEFAULT_SHADERS_H_
#define ETH_DEFAULT_SHADERS_H_

#include <string>

namespace ETHGlobal {

const std::string Cg_defaultVS_cg = 
"// sprite and screen properties:\n" \
"uniform float4x4 viewMatrix;     // orthogonal matrix for the screen space\n" \
"uniform float4x4 rotationMatrix; // sprite rotation matrix\n" \
"uniform float2 screenSize;       // current screen size\n" \
"uniform float2 size;             // sprite width and height\n" \
"uniform float2 entityPos;        // sprite position\n" \
"uniform float2 center;           // sprite origin (in pixels)\n" \
"uniform float2 bitmapSize;       // sprite size\n" \
"uniform float2 rectPos;          // texture rect cut position (in pixels)\n" \
"uniform float2 rectSize;         // texture rect cut size (in pixels)\n" \
"uniform float2 scroll;           // texture scroll parameter\n" \
"uniform float2 multiply;         // texture multiply\n" \
"uniform float4 color0;           // left-top vertex color\n" \
"uniform float4 color1;           // right-top vertex color\n" \
"uniform float4 color2;           // left-bottom vertex color\n" \
"uniform float4 color3;           // right-bottom vertex color\n" \
"uniform float2 flipAdd;          // this is added to the vertex position to flip it\n" \
"uniform float2 flipMul;          // this is multiplied by the vertex position do flip it\n" \
"uniform float2 cameraPos;\n" \
"uniform float depth;\n" \
"\n" \
"// returns the sprite vertex position according to it's\n" \
"// origin in screen space and rotation angle\n" \
"float4 transformSprite(float3 position)\n" \
"{\n" \
"	// flips the sprite (if the parameters are set to flip)\n" \
"	float4 newPos = float4(position, 1);\n" \
"\n" \
"	// scales and position the sprite center\n" \
"	newPos = newPos * float4(size,1,1) - float4(center, 0, 0);\n" \
"\n" \
"	// rotates the sprite\n" \
"	newPos = mul(rotationMatrix, newPos);\n" \
"\n" \
"	// position the sprite according to the screen space\n" \
"	newPos += float4(entityPos,0,0)-float4(screenSize/2,0,0)-float4(cameraPos,0,0);\n" \
"\n" \
"	// inverts the y coordinate\n" \
"	newPos *= float4(1,-1,1,1);\n" \
"	return mul(viewMatrix, newPos);\n" \
"}\n" \
"\n" \
"// returns the texture coordinate according to the rect\n" \
"float2 transformCoord(float2 texCoord)\n" \
"{\n" \
"	// adjusts the texture coordinate according to the cut-rect\n" \
"	float2 newCoord = texCoord * (rectSize/bitmapSize);\n" \
"	newCoord += (rectPos/bitmapSize);\n" \
"\n" \
"	// scrolls and multiplies the texture\n" \
"	return (newCoord);\n" \
"}\n" \
"\n" \
"// GS_SPRITE::DrawSprite:\n" \
"// main sprite program\n" \
"void main(	float3 position : POSITION,\n" \
"			float2 texCoord : TEXCOORD0,\n" \
"			out float4 oPosition : POSITION,\n" \
"			out float4 oColor    : COLOR0,\n" \
"			out float2 oTexCoord0 : TEXCOORD0,\n" \
"			out float2 oTexCoord1 : TEXCOORD1)\n" \
"{\n" \
"	float4 outPos = transformSprite(position);\n" \
"	outPos.z = 1-depth;\n" \
"	oPosition = outPos;\n" \
"	float2 coord = transformCoord(texCoord);\n" \
"	oTexCoord0 = oTexCoord1 = coord;\n" \
"	oColor = color0;\n" \
"}\n" \
"\n" \
"\n";

const std::string Cg_dynaShadowVS_cg = 
"// sprite and screen properties:\n" \
"uniform float4x4 viewMatrix;     // orthogonal matrix for the screen space\n" \
"uniform float4x4 rotationMatrix; // sprite rotation matrix\n" \
"uniform float2 screenSize;       // current screen size\n" \
"uniform float2 size;             // sprite width and height\n" \
"uniform float2 entityPos;        // sprite position\n" \
"uniform float2 center;           // sprite origin (in pixels)\n" \
"uniform float2 bitmapSize;       // sprite size\n" \
"uniform float2 rectPos;          // texture rect cut position (in pixels)\n" \
"uniform float2 rectSize;         // texture rect cut size (in pixels)\n" \
"uniform float2 scroll;           // texture scroll parameter\n" \
"uniform float2 multiply;         // texture multiply\n" \
"uniform float4 color0;           // left-top vertex color\n" \
"uniform float4 color1;           // right-top vertex color\n" \
"uniform float4 color2;           // left-bottom vertex color\n" \
"uniform float4 color3;           // right-bottom vertex color\n" \
"uniform float2 flipAdd;          // this is added to the vertex position to flip it\n" \
"uniform float2 flipMul;          // this is multiplied by the vertex position do flip it\n" \
"uniform float2 cameraPos;\n" \
"uniform float depth;\n" \
"uniform float shadowLength;\n" \
"uniform float lightRange;\n" \
"uniform float3 lightPos;\n" \
"uniform float entityZ;\n" \
"uniform float shadowZ;\n" \
"\n" \
"struct SPRITE_TRANSFORM\n" \
"{\n" \
"	float4 position;\n" \
"	float2 vertPos;\n" \
"};\n" \
"\n" \
"SPRITE_TRANSFORM transformSprite(float3 position, float v)\n" \
"{\n" \
"	// 'extrude' equals 1 if this is one of the upper vertices\n" \
"	const float extrude = 1-v;\n" \
"\n" \
"	SPRITE_TRANSFORM r;\n" \
"	float4 newPos = float4(position, 1);\n" \
"	newPos = newPos * float4(size,1,1) - float4(center, 0, 0);\n" \
"	newPos = mul(rotationMatrix, newPos)+float4(entityPos,0,0);\n" \
"\n" \
"	// extrude the shadow\n" \
"	float2 lightVec = normalize(entityPos-lightPos.xy);\n" \
"	float2 shadowDir = normalize(newPos.xy-lightPos.xy)*shadowLength*extrude;\n" \
"	\n" \
"	// push back the shadow a little bit so it won't look odd\n" \
"	newPos += float4(-lightVec*((shadowLength/6)-entityZ),0,0);\n" \
"	newPos.x += (shadowDir.x);\n" \
"	newPos.y += (shadowDir.y);\n" \
"\n" \
"	r.vertPos = newPos.xy;\n" \
"\n" \
"	// project the vertex to the screen\n" \
"	newPos -= float4(screenSize/2,0,0)+float4(cameraPos,0,0);\n" \
"	newPos.y =-newPos.y+shadowZ;\n" \
"	r.position = mul(viewMatrix, newPos);\n" \
"	return r;\n" \
"}\n" \
"\n" \
"// GS_SPRITE::DrawSprite:\n" \
"// main sprite program\n" \
"void main(	float3 position : POSITION,\n" \
"			float2 texCoord : TEXCOORD0,\n" \
"			out float4 oPosition : POSITION,\n" \
"			out float4 oColor    : COLOR0,\n" \
"			out float2 oTexCoord0 : TEXCOORD0)\n" \
"{\n" \
"	SPRITE_TRANSFORM outPos = transformSprite(position, texCoord.y);\n" \
"	outPos.position.z = 1-depth;\n" \
"	oPosition = outPos.position;\n" \
"	oTexCoord0 = texCoord;\n" \
"	oColor = color0;\n" \
"}\n" \
"\n" \
"\n";

const std::string Cg_hAmbientVS_cg = 
"// sprite and screen properties:\n" \
"uniform float4x4 viewMatrix;     // orthogonal matrix for the screen space\n" \
"uniform float4x4 rotationMatrix; // sprite rotation matrix\n" \
"uniform float2 screenSize;       // current screen size\n" \
"uniform float2 size;             // sprite width and height\n" \
"uniform float2 entityPos;        // sprite position\n" \
"uniform float2 center;           // sprite origin (in pixels)\n" \
"uniform float2 bitmapSize;       // sprite size\n" \
"uniform float2 rectPos;          // texture rect cut position (in pixels)\n" \
"uniform float2 rectSize;         // texture rect cut size (in pixels)\n" \
"uniform float2 scroll;           // texture scroll parameter\n" \
"uniform float2 multiply;         // texture multiply\n" \
"uniform float4 color0;           // left-top vertex color\n" \
"uniform float4 color1;           // right-top vertex color\n" \
"uniform float4 color2;           // left-bottom vertex color\n" \
"uniform float4 color3;           // right-bottom vertex color\n" \
"uniform float2 flipAdd;          // this is added to the vertex position to flip it\n" \
"uniform float2 flipMul;          // this is multiplied by the vertex position do flip it\n" \
"uniform float2 cameraPos;\n" \
"uniform float depth;\n" \
"\n" \
"uniform float3 parallaxOrigin_verticalIntensity;\n" \
"uniform float parallaxIntensity;\n" \
"uniform float3 entityPos3D;\n" \
"\n" \
"\n" \
"\n" \
"\n" \
"\n" \
"\n" \
"\n" \
"float2 computeParallaxOffset(const float2 vertPos)\n" \
"{\n" \
"\n" \
"	const float2 parallaxOrigin = parallaxOrigin_verticalIntensity.xy;\n" \
"	return ((vertPos - parallaxOrigin) / screenSize.x) * entityPos3D.z * parallaxIntensity;\n" \
"}\n" \
"\n" \
"// returns the sprite vertex position according to it's\n" \
"// origin in screen space and rotation angle\n" \
"float4 transformSprite(float3 position)\n" \
"{\n" \
"	// flips the sprite (if the parameters are set to flip)\n" \
"	float4 newPos = float4(position, 1) * float4(flipMul,1,1) + float4(flipAdd,0,0);\n" \
"\n" \
"	// scales and position the sprite center\n" \
"	newPos = newPos * float4(size,1,1) - float4(center, 0, 0);\n" \
"\n" \
"	// rotates the sprite\n" \
"	newPos = mul(rotationMatrix, newPos);\n" \
"\n" \
"	// position the sprite according to the screen space\n" \
"	newPos += float4(entityPos,0,0) - float4(cameraPos,0,0);\n" \
"	newPos += float4(computeParallaxOffset(newPos.xy),0,0) - float4(screenSize / 2,0,0);\n" \
"\n" \
"	// inverts the y coordinate\n" \
"	newPos *= float4(1,-1,1,1);\n" \
"	return mul(viewMatrix, newPos);\n" \
"}\n" \
"\n" \
"// returns the texture coordinate according to the rect\n" \
"float2 transformCoord(float2 texCoord)\n" \
"{\n" \
"	// adjusts the texture coordinate according to the cut-rect\n" \
"	float2 newCoord = texCoord * (rectSize / bitmapSize);\n" \
"	newCoord += (rectPos / bitmapSize);\n" \
"\n" \
"	// scrolls and multiplies the texture\n" \
"	return (newCoord);\n" \
"}\n" \
"\n" \
"void main(	float3 position : POSITION,\n" \
"			float2 texCoord : TEXCOORD0,\n" \
"			out float4 oPosition : POSITION,\n" \
"			out float4 oColor    : COLOR0,\n" \
"			out float2 oTexCoord0 : TEXCOORD0,\n" \
"			out float2 oTexCoord1 : TEXCOORD1)\n" \
"{\n" \
"	float4 outPos = transformSprite(position);\n" \
"	depth = 1 - depth;\n" \
"	outPos.z = depth;\n" \
"	oPosition = outPos;\n" \
"	float2 coord = transformCoord(texCoord);\n" \
"	oTexCoord0 = coord;\n" \
"	oTexCoord1 = texCoord;\n" \
"	oColor = color0;\n" \
"}\n" \
"\n" \
"\n";

const std::string Cg_hPixelLightDiff_cg = 
"void main(float2 texCoord : TEXCOORD0,\n" \
"		  float3 pixelPos3D : TEXCOORD1,\n" \
"		  float4 color0     : COLOR0,\n" \
"		  out float4 oColor : COLOR,\n" \
"		  uniform sampler2D diffuse,\n" \
"		  uniform sampler2D normalMap,\n" \
"		  uniform float3 lightPos,\n" \
"		  uniform float squaredRange,\n" \
"		  uniform float4 lightColor)\n" \
"{\n" \
"	const float4 diffuseColor = tex2D(diffuse, texCoord);\n" \
"	float3 normalColor = tex2D(normalMap, texCoord);\n" \
"\n" \
"	const float3 lightVec = pixelPos3D-lightPos;\n" \
"	lightColor.a = 1.0f;\n" \
"\n" \
"	normalColor = -normalize(2*(normalColor-0.5));\n" \
"\n" \
"	\n" \
"	\n" \
"	float diffuseLight = dot(normalize(lightVec), normalColor);\n" \
"\n" \
"	float squaredDist = dot(lightVec,lightVec);\n" \
"\n" \
"	squaredRange = max(squaredDist, squaredRange);\n" \
"	const float attenBias = 1-(squaredDist/squaredRange);\n" \
"\n" \
"	oColor = (diffuseColor*color0*diffuseLight*attenBias*lightColor)*diffuseColor.w;\n" \
"}\n" \
"\n" \
"\n";

const std::string Cg_hPixelLightSpec_cg = 
"void main(float2 texCoord : TEXCOORD0,\n" \
"		  float3 pixelPos3D : TEXCOORD1,\n" \
"		  float4 color0     : COLOR0,\n" \
"		  out float4 oColor : COLOR,\n" \
"		  uniform sampler2D diffuse,\n" \
"		  uniform sampler2D normalMap,\n" \
"		  uniform sampler2D glossMap,\n" \
"		  uniform float specularPower,\n" \
"		  uniform float3 lightPos,\n" \
"		  uniform float squaredRange,\n" \
"		  uniform float4 lightColor,\n" \
"		  uniform float3 fakeEyePos,\n" \
"		  uniform float specularBrightness)\n" \
"{\n" \
"	const float4 diffuseColor = tex2D(diffuse, texCoord);\n" \
"	float3 normalColor = tex2D(normalMap, texCoord).xyz;\n" \
"	const float4 glossColor = tex2D(glossMap, texCoord)*specularBrightness;\n" \
"\n" \
"	const float3 lightVec = (pixelPos3D-lightPos);\n" \
"	const float3 eyeVec = (pixelPos3D-fakeEyePos);\n" \
"	\n" \
"	const float lightVecLength = length(lightVec);\n" \
"	const float eyeVecLength = length(eyeVec);\n" \
"	const float3 halfVec = normalize(lightVec/lightVecLength+eyeVec/eyeVecLength);\n" \
"	lightColor.a = 1.0f;\n" \
"\n" \
"	normalColor = -normalize(2*(normalColor-0.5));\n" \
"	\n" \
"	\n" \
"\n" \
"	float diffuseLight = dot(lightVec/lightVecLength, (normalColor));\n" \
"\n" \
"	float squaredDist = dot(lightVec,lightVec);\n" \
"\n" \
"	squaredRange = max(squaredDist, squaredRange);\n" \
"	const float attenBias = 1-(squaredDist/squaredRange);\n" \
"\n" \
"	const float4 specular = lightColor*pow(saturate(dot(normalColor, halfVec)), specularPower);\n" \
"\n" \
"	oColor = ((diffuseColor*color0*diffuseLight*lightColor)+specular*diffuseColor.w*glossColor)*attenBias;\n" \
"}\n" \
"\n" \
"\n";

const std::string Cg_hPixelLightVS_cg = 
"// sprite and screen properties\n" \
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
"\n" \
"uniform float3 parallaxOrigin_verticalIntensity;\n" \
"uniform float parallaxIntensity;\n" \
"uniform float3 entityPos3D;\n" \
"\n" \
"\n" \
"\n" \
"\n" \
"\n" \
"float2 computeParallaxOffset(const float2 vertPos)\n" \
"{\n" \
"	const float2 parallaxOrigin = parallaxOrigin_verticalIntensity.xy;\n" \
"	return ((vertPos - parallaxOrigin) / screenSize.x) * entityPos3D.z * parallaxIntensity;\n" \
"}\n" \
"\n" \
"struct SPRITE_TRANSFORM\n" \
"{\n" \
"	float4 position;\n" \
"	float2 vertPos;\n" \
"};\n" \
"SPRITE_TRANSFORM transformSprite_ppl(float3 position)\n" \
"{\n" \
"	SPRITE_TRANSFORM r;\n" \
"	float4 newPos = float4(position, 1);\n" \
"	newPos = newPos * float4(size,1,1) - float4(center, 0, 0);\n" \
"	newPos = mul(rotationMatrix, newPos) + float4(entityPos,0,0);\n" \
"	r.vertPos = newPos.xy / screenSize;\n" \
"\n" \
"	// project the vertex on the screen\n" \
"	newPos -= float4(cameraPos,0,0);\n" \
"	newPos += float4(computeParallaxOffset(newPos),0,0) - float4(screenSize / 2,0,0);\n" \
"	newPos *= float4(1,-1,1,1);\n" \
"	r.position = mul(viewMatrix, newPos);\n" \
"	return r;\n" \
"}\n" \
"\n" \
"// returns the texture coordinate according to the rect\n" \
"float2 transformCoord(float2 texCoord)\n" \
"{\n" \
"	float2 newCoord = texCoord * (rectSize / bitmapSize);\n" \
"	newCoord += (rectPos / bitmapSize);\n" \
"	return (newCoord + (scroll / bitmapSize)) * multiply;\n" \
"}\n" \
"\n" \
"void main(float3 position : POSITION,\n" \
"			float2 texCoord : TEXCOORD0,\n" \
"			out float4 oPosition : POSITION,\n" \
"			out float4 oColor    : COLOR0,\n" \
"			out float2 oTexCoord : TEXCOORD0,\n" \
"			out float3 oVertPos3D : TEXCOORD1,\n" \
"			uniform float3 topLeft3DPos)\n" \
"			\n" \
"{\n" \
"	SPRITE_TRANSFORM transform = transformSprite_ppl(position);\n" \
"\n" \
"	transform.position.z = 1 - depth;\n" \
"\n" \
"	oPosition = transform.position;\n" \
"	oVertPos3D = topLeft3DPos + (position * float3(size,1));\n" \
"	oTexCoord = transformCoord(texCoord);\n" \
"	oColor = color0;\n" \
"}\n" \
"\n" \
"\n";

const std::string Cg_hVertexLightShader_cg = 
"// sprite and screen properties\n" \
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
"uniform float lightIntensity;\n" \
"\n" \
"uniform float3 pivotAdjust;\n" \
"uniform float3 lightPos;\n" \
"uniform float lightRange;\n" \
"uniform float4 lightColor;\n" \
"\n" \
"uniform float3 parallaxOrigin_verticalIntensity;\n" \
"uniform float parallaxIntensity;\n" \
"uniform float3 entityPos3D;\n" \
"\n" \
"float2 computeParallaxOffset(const float2 vertPos)\n" \
"{\n" \
"	const float2 parallaxOrigin = parallaxOrigin_verticalIntensity.xy;\n" \
"	return ((vertPos - parallaxOrigin) / screenSize.x) * entityPos3D.z * parallaxIntensity;\n" \
"}\n" \
"\n" \
"struct SPRITE_TRANSFORM\n" \
"{\n" \
"	float4 position;\n" \
"	float2 vertPos;\n" \
"};\n" \
"SPRITE_TRANSFORM transformSprite(float3 position)\n" \
"{\n" \
"	SPRITE_TRANSFORM r;\n" \
"	float4 newPos = float4(position, 1);\n" \
"	newPos = newPos * float4(size,1,1) - float4(center, 0, 0);\n" \
"	newPos = mul(rotationMatrix, newPos) + float4(entityPos,0,0);\n" \
"	r.vertPos = newPos.xy / screenSize;\n" \
"\n" \
"	// project the vertex on the screen\n" \
"	newPos -= float4(cameraPos,0,0);\n" \
"	newPos += float4(computeParallaxOffset(newPos),0,0) - float4(screenSize / 2,0,0);\n" \
"	newPos *= float4(1,-1,1,1);\n" \
"	r.position = mul(viewMatrix, newPos);\n" \
"	return r;\n" \
"}\n" \
"\n" \
"// returns the texture coordinate according to the rect\n" \
"float2 transformCoord(float2 texCoord)\n" \
"{\n" \
"	float2 newCoord = texCoord * (rectSize/bitmapSize);\n" \
"	newCoord += (rectPos/bitmapSize);\n" \
"	return (newCoord+(scroll/bitmapSize))*multiply;\n" \
"}\n" \
"\n" \
"// main sprite program (with per-pixel lighting)\n" \
"void sprite_pvl(float3 position : POSITION,\n" \
"				float2 texCoord : TEXCOORD0,\n" \
"				out float4 oPosition : POSITION,\n" \
"				out float4 oColor    : COLOR0,\n" \
"				out float2 oTexCoord : TEXCOORD0,\n" \
"				out float2 oVertPos : TEXCOORD1,\n" \
"				uniform float3 topLeft3DPos)\n" \
"{\n" \
"	SPRITE_TRANSFORM transform = transformSprite(position);\n" \
"	transform.position.z = 1-depth;\n" \
"	oPosition = transform.position;\n" \
"	oVertPos = transform.vertPos;\n" \
"	oTexCoord = transformCoord(texCoord);\n" \
"\n" \
"	lightColor.a = 1.0;\n" \
"\n" \
"	float3 vertPos3D = topLeft3DPos + (position*float3(size,1));\n" \
"\n" \
"	float3 lightVec  = vertPos3D-lightPos;\n" \
"	float squaredDist = dot(lightVec,lightVec);\n" \
"\n" \
"	float3 normalColor = float3(0.0,0.0,-1.0);\n" \
"	float diffuseLight = dot(normalize(lightVec), normalColor);\n" \
"\n" \
"	float squaredRange = lightRange*lightRange;\n" \
"	squaredRange = max(squaredDist, squaredRange);\n" \
"	float attenBias = 1-(squaredDist/squaredRange);\n" \
"	oColor = color0*diffuseLight*attenBias*lightColor*lightIntensity;\n" \
"}\n" \
"\n" \
"\n";

const std::string Cg_particleVS_cg = 
"// the following global uniform parameters are set automaticaly\n" \
"// by the GameSpaceLib runtime\n" \
"\n" \
"// sprite and screen properties:\n" \
"uniform float4x4 viewMatrix;     // orthogonal matrix for the screen space\n" \
"uniform float4x4 rotationMatrix; // sprite rotation matrix\n" \
"uniform float2 screenSize;       // current screen size\n" \
"uniform float2 size;             // sprite width and height\n" \
"uniform float2 entityPos;        // sprite position\n" \
"uniform float2 center;           // sprite origin (in pixels)\n" \
"uniform float2 bitmapSize;       // sprite size\n" \
"uniform float2 rectPos;          // texture rect cut position (in pixels)\n" \
"uniform float2 rectSize;         // texture rect cut size (in pixels)\n" \
"uniform float2 scroll;           // texture scroll parameter\n" \
"uniform float2 multiply;         // texture multiply\n" \
"uniform float4 color0;           // left-top vertex color\n" \
"uniform float4 color1;           // right-top vertex color\n" \
"uniform float4 color2;           // left-bottom vertex color\n" \
"uniform float4 color3;           // right-bottom vertex color\n" \
"uniform float2 flipAdd;          // this is added to the vertex position to flip it\n" \
"uniform float2 flipMul;          // this is multiplied by the vertex position do flip it\n" \
"uniform float2 cameraPos;\n" \
"uniform float depth;\n" \
"\n" \
"// returns the sprite vertex position according to it's\n" \
"// origin in screen space and rotation angle\n" \
"float4 transformSprite(float3 position)\n" \
"{\n" \
"	// flips the sprite (if the parameters are set to flip)\n" \
"	float4 newPos = float4(position, 1);\n" \
"\n" \
"	// scales and position the sprite center\n" \
"	newPos = newPos * float4(size,1,1) - float4(center, 0, 0);\n" \
"\n" \
"	// rotates the sprite\n" \
"	newPos = mul(rotationMatrix, newPos);\n" \
"\n" \
"	// position the sprite according to the screen space\n" \
"	newPos += float4(entityPos,0,0)-float4(screenSize/2,0,0)-float4(cameraPos,0,0);\n" \
"\n" \
"	// inverts the y coordinate\n" \
"	newPos *= float4(1,-1,1,1);\n" \
"	return mul(viewMatrix, newPos);\n" \
"}\n" \
"\n" \
"// returns the texture coordinate according to the rect\n" \
"float2 transformCoord(float2 texCoord)\n" \
"{\n" \
"	// adjusts the texture coordinate according to the cut-rect\n" \
"	float2 newCoord = texCoord * (rectSize/bitmapSize);\n" \
"	newCoord += (rectPos/bitmapSize);\n" \
"\n" \
"	// scrolls and multiplies the texture\n" \
"	return (newCoord);\n" \
"}\n" \
"\n" \
"void main(	float3 position : POSITION,\n" \
"			float2 texCoord : TEXCOORD0,\n" \
"			out float4 oPosition : POSITION,\n" \
"			out float4 oColor    : COLOR0,\n" \
"			out float2 oTexCoord0 : TEXCOORD0)\n" \
"{\n" \
"	float4 outPos = transformSprite(position);\n" \
"	outPos.z = 1-depth;\n" \
"	oPosition = outPos;\n" \
"	float2 coord = transformCoord(texCoord);\n" \
"	oTexCoord0 = coord;\n" \
"	oColor = color0;\n" \
"}\n" \
"\n";

const std::string Cg_vAmbientVS_cg = 
"// sprite and screen properties:\n" \
"uniform float4x4 viewMatrix;     // orthogonal matrix for the screen space\n" \
"uniform float4x4 rotationMatrix; // sprite rotation matrix\n" \
"uniform float2 screenSize;       // current screen size\n" \
"uniform float2 size;             // sprite width and height\n" \
"uniform float2 entityPos;        // sprite position\n" \
"uniform float2 center;           // sprite origin (in pixels)\n" \
"uniform float2 bitmapSize;       // sprite size\n" \
"uniform float2 rectPos;          // texture rect cut position (in pixels)\n" \
"uniform float2 rectSize;         // texture rect cut size (in pixels)\n" \
"uniform float2 scroll;           // texture scroll parameter\n" \
"uniform float2 multiply;         // texture multiply\n" \
"uniform float4 color0;           // left-top vertex color\n" \
"uniform float4 color1;           // right-top vertex color\n" \
"uniform float4 color2;           // left-bottom vertex color\n" \
"uniform float4 color3;           // right-bottom vertex color\n" \
"uniform float2 flipAdd;          // this is added to the vertex position to flip it\n" \
"uniform float2 flipMul;          // this is multiplied by the vertex position do flip it\n" \
"uniform float2 cameraPos;\n" \
"uniform float depth;\n" \
"\n" \
"uniform float3 parallaxOrigin_verticalIntensity;\n" \
"uniform float parallaxIntensity;\n" \
"uniform float4 entityPos3D;\n" \
"\n" \
"\n" \
"\n" \
"float computeVerticalOffsetInPixels(const float posY, const float rectSizeY)\n" \
"{\n" \
"	return ((1 - posY) * rectSizeY);\n" \
"}\n" \
"\n" \
"float2 computeParallaxOffset(const float2 vertPos, float3 inPosition)\n" \
"{\n" \
"	const float vOffset = computeVerticalOffsetInPixels(inPosition.y, rectSize.y);\n" \
"	const float2 parallaxOrigin = parallaxOrigin_verticalIntensity.xy;\n" \
"	const float verticalIntensity = parallaxOrigin_verticalIntensity.z;\n" \
"	return ((vertPos - parallaxOrigin) / screenSize.x) * (entityPos3D.z + (vOffset * verticalIntensity)) * parallaxIntensity;\n" \
"}\n" \
"\n" \
"// returns the sprite vertex position according to it's\n" \
"// origin in screen space and rotation angle\n" \
"float4 transformSprite(float3 position)\n" \
"{\n" \
"	float4 newPos = float4(position, 1) * float4(flipMul,1,1) + float4(flipAdd,0,0);\n" \
"\n" \
"	// scales and position the sprite center\n" \
"	newPos = newPos * float4(size,1,1) - float4(center, 0, 0);\n" \
"\n" \
"	// rotates the sprite\n" \
"	newPos = mul(rotationMatrix, newPos);\n" \
"\n" \
"	// position the sprite according to screen space\n" \
"	newPos += float4(entityPos,0,0) - float4(cameraPos,0,0);\n" \
"	newPos += float4(computeParallaxOffset(newPos.xy, position),0,0) - float4(screenSize / 2,0,0);\n" \
"\n" \
"	// inverts the y coordinate\n" \
"	newPos *= float4(1,-1,1,1);\n" \
"	return mul(viewMatrix, newPos);\n" \
"}\n" \
"\n" \
"// returns the texture coordinate according to the rect\n" \
"float2 transformCoord(float2 texCoord)\n" \
"{\n" \
"	// adjusts the texture coordinate according to the cut-rect\n" \
"	float2 newCoord = texCoord * (rectSize / bitmapSize);\n" \
"	newCoord += (rectPos / bitmapSize);\n" \
"\n" \
"	// scrolls and multiplies the texture\n" \
"	return (newCoord);\n" \
"}\n" \
"\n" \
"void main(  float3 inPosition : POSITION,\n" \
"			float2 texCoord : TEXCOORD0,\n" \
"			out float4 oPosition : POSITION,\n" \
"			out float4 oColor    : COLOR0,\n" \
"			out float2 oTexCoord0 : TEXCOORD0,\n" \
"			out float2 oTexCoord1 : TEXCOORD1,\n" \
"			uniform float spaceLength)\n" \
"{\n" \
"	float4 outPos = transformSprite(inPosition);\n" \
"	outPos.z = (1 - depth) - ((computeVerticalOffsetInPixels(inPosition.y, rectSize.y)) / spaceLength);\n" \
"	oPosition = outPos;\n" \
"	float2 coord = transformCoord(texCoord);\n" \
"	oTexCoord0 = coord;\n" \
"	oTexCoord1 = texCoord;\n" \
"	oColor = color0;\n" \
"}\n" \
"\n" \
"\n";

const std::string Cg_vPixelLightDiff_cg = 
"void main(float2 texCoord : TEXCOORD0,\n" \
"		  float3 pixelPos3D : TEXCOORD1,\n" \
"		  float4 color0     : COLOR0,\n" \
"		  out float4 oColor : COLOR,\n" \
"		  uniform sampler2D diffuse,\n" \
"		  uniform sampler2D normalMap,\n" \
"		  uniform float3 lightPos,\n" \
"		  uniform float squaredRange,\n" \
"		  uniform float4 lightColor)\n" \
"{\n" \
"	const float4 diffuseColor = tex2D(diffuse, texCoord);\n" \
"	float3 normalColor = tex2D(normalMap, texCoord).xyz;\n" \
"\n" \
"	const float3 lightVec = pixelPos3D-lightPos;\n" \
"	lightColor.a = 1.0f;\n" \
"\n" \
"	normalColor = -normalize(2*(normalColor-0.5));\n" \
"	normalColor = normalColor.xzy;\n" \
"	normalColor.z *=-1;\n" \
"\n" \
"	float diffuseLight = dot(normalize(lightVec), normalColor);\n" \
"\n" \
"	float squaredDist = dot(lightVec,lightVec);\n" \
"\n" \
"	squaredRange = max(squaredDist, squaredRange);\n" \
"	const float attenBias = 1-(squaredDist/squaredRange);\n" \
"\n" \
"	oColor = diffuseColor*color0*diffuseLight*attenBias*lightColor;\n" \
"}\n" \
"\n" \
"\n";

const std::string Cg_vPixelLightSpec_cg = 
"void main(float2 texCoord : TEXCOORD0,\n" \
"		  float3 pixelPos3D : TEXCOORD1,\n" \
"		  float4 color0   : COLOR0,\n" \
"		  out float4 oColor : COLOR,\n" \
"		  uniform sampler2D diffuse,\n" \
"		  uniform sampler2D normalMap,\n" \
"		  uniform sampler2D glossMap,\n" \
"		  uniform float specularPower,\n" \
"		  uniform float3 lightPos,\n" \
"		  uniform float squaredRange,\n" \
"		  uniform float4 lightColor,\n" \
"		  uniform float3 fakeEyePos,\n" \
"		  uniform float specularBrightness)\n" \
"{\n" \
"	const float4 diffuseColor = tex2D(diffuse, texCoord);\n" \
"	float3 normalColor = tex2D(normalMap, texCoord);\n" \
"	const float4 glossColor = tex2D(glossMap, texCoord)*specularBrightness;\n" \
"\n" \
"	const float3 lightVec  = pixelPos3D-lightPos;\n" \
"	const float3 eyeVec  = pixelPos3D-fakeEyePos;\n" \
"	\n" \
"	const float lightVecLength  = length(lightVec);\n" \
"	const float eyeVecLength  = length(eyeVec);\n" \
"	const float3 halfVec = normalize(lightVec/lightVecLength+eyeVec/eyeVecLength);\n" \
"	lightColor.a = 1.0f;\n" \
"\n" \
"	normalColor = -normalize(2*(normalColor-0.5));\n" \
"	normalColor = normalColor.xzy;\n" \
"	normalColor.z *=-1;\n" \
"\n" \
"	float diffuseLight = dot(lightVec/lightVecLength, (normalColor));\n" \
"\n" \
"	float squaredDist = dot(lightVec,lightVec);\n" \
"\n" \
"	squaredRange = max(squaredDist, squaredRange);\n" \
"	const float attenBias = 1-(squaredDist/squaredRange);\n" \
"\n" \
"	const float4 specular = lightColor*pow(saturate(dot(normalColor, halfVec)), specularPower);\n" \
"\n" \
"	oColor = ((diffuseColor*color0*diffuseLight*lightColor)+specular*diffuseColor.w*glossColor)*attenBias;\n" \
"}\n" \
"\n" \
"\n";

const std::string Cg_vPixelLightVS_cg = 
"// sprite and screen properties\n" \
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
"\n" \
"uniform float3 parallaxOrigin_verticalIntensity;\n" \
"uniform float parallaxIntensity;\n" \
"uniform float3 entityPos3D;\n" \
"float computeVerticalOffsetInPixels(const float posY, const float rectSizeY)\n" \
"{\n" \
"	return ((1 - posY) * rectSizeY);\n" \
"}\n" \
"\n" \
"float2 computeParallaxOffset(const float2 vertPos, float3 inPosition)\n" \
"{\n" \
"	const float vOffset = computeVerticalOffsetInPixels(inPosition.y, rectSize.y);\n" \
"	const float2 parallaxOrigin = parallaxOrigin_verticalIntensity.xy;\n" \
"	const float verticalIntensity = parallaxOrigin_verticalIntensity.z;\n" \
"	return ((vertPos - parallaxOrigin) / screenSize.x) * (entityPos3D.z + (vOffset * verticalIntensity)) * parallaxIntensity;\n" \
"}\n" \
"\n" \
"float4 transformSprite(float3 position)\n" \
"{\n" \
"	float4 newPos = float4(position, 1);\n" \
"\n" \
"	// scales and position the sprite center\n" \
"	newPos = newPos * float4(size,1,1) - float4(center, 0, 0);\n" \
"\n" \
"	// rotates the sprite\n" \
"	newPos = mul(rotationMatrix, newPos);\n" \
"\n" \
"	// position the sprite according to screen space\n" \
"	newPos += float4(entityPos,0,0) - float4(cameraPos,0,0);\n" \
"	newPos += float4(computeParallaxOffset(newPos.xy, position),0,0) - float4(screenSize / 2,0,0);\n" \
"\n" \
"	// inverts the y coordinate\n" \
"	newPos *= float4(1,-1,1,1);\n" \
"	return mul(viewMatrix, newPos);\n" \
"}\n" \
"\n" \
"// returns the texture coordinate according to the rect\n" \
"float2 transformCoord(float2 texCoord)\n" \
"{\n" \
"	float2 newCoord = texCoord * (rectSize / bitmapSize);\n" \
"	newCoord += (rectPos / bitmapSize);\n" \
"	return (newCoord + (scroll / bitmapSize)) * multiply;\n" \
"}\n" \
"\n" \
"void main(float3 position : POSITION,\n" \
"			float2 texCoord : TEXCOORD0,\n" \
"			out float4 oPosition : POSITION,\n" \
"			out float4 oColor    : COLOR0,\n" \
"			out float2 oTexCoord : TEXCOORD0,\n" \
"			out float3 oVertPos3D : TEXCOORD1,\n" \
"			uniform float3 topLeft3DPos,\n" \
"			uniform float spaceLength)\n" \
"{\n" \
"	float4 outPos = transformSprite(position);\n" \
"	outPos.z = (1 - depth) - ((computeVerticalOffsetInPixels(position.y, rectSize.y)) / spaceLength);\n" \
"\n" \
"	oPosition = outPos;\n" \
"	oVertPos3D = topLeft3DPos + (float3(position.x,0,position.y) * float3(size.x,0,-size.y));\n" \
"	oTexCoord = transformCoord(texCoord);\n" \
"	oColor = color0;\n" \
"}\n" \
"\n" \
"\n";

const std::string Cg_vVertexLightShader_cg = 
"// sprite and screen properties\n" \
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
"uniform float lightIntensity;\n" \
"\n" \
"uniform float3 pivotAdjust;\n" \
"uniform float3 lightPos;\n" \
"uniform float lightRange;\n" \
"uniform float4 lightColor;\n" \
"\n" \
"\n" \
"\n" \
"uniform float3 parallaxOrigin_verticalIntensity;\n" \
"uniform float parallaxIntensity;\n" \
"uniform float3 entityPos3D;\n" \
"float computeVerticalOffsetInPixels(const float posY, const float rectSizeY)\n" \
"{\n" \
"	return ((1 - posY) * rectSizeY);\n" \
"}\n" \
"\n" \
"float2 computeParallaxOffset(const float2 vertPos, float3 inPosition)\n" \
"{\n" \
"	const float2 parallaxOrigin = parallaxOrigin_verticalIntensity.xy;\n" \
"	const float verticalIntensity = parallaxOrigin_verticalIntensity.z;\n" \
"	const float vOffset = computeVerticalOffsetInPixels(inPosition.y, rectSize.y);\n" \
"	return ((vertPos - parallaxOrigin) / screenSize.x) * (entityPos3D.z + (vOffset * verticalIntensity)) * parallaxIntensity;\n" \
"}\n" \
"\n" \
"struct SPRITE_TRANSFORM\n" \
"{\n" \
"	float4 position;\n" \
"	float2 vertPos;\n" \
"};\n" \
"SPRITE_TRANSFORM transformSprite(float3 position)\n" \
"{\n" \
"	SPRITE_TRANSFORM r;\n" \
"	float4 newPos = float4(position, 1);\n" \
"	newPos = newPos * float4(size,1,1) - float4(center, 0, 0);\n" \
"	newPos = mul(rotationMatrix, newPos) + float4(entityPos,0,0);\n" \
"	r.vertPos = newPos.xy/screenSize;\n" \
"\n" \
"	// project the vertex on the screen\n" \
"	newPos -= float4(cameraPos,0,0);\n" \
"	newPos += float4(computeParallaxOffset(newPos.xy, position),0,0) - float4(screenSize / 2,0,0);\n" \
"	newPos *= float4(1,-1,1,1);\n" \
"	r.position = mul(viewMatrix, newPos);\n" \
"	return r;\n" \
"}	\n" \
"\n" \
"// returns the texture coordinate according to the rect\n" \
"float2 transformCoord(float2 texCoord)\n" \
"{\n" \
"	float2 newCoord = texCoord * (rectSize/bitmapSize);\n" \
"	newCoord += (rectPos/bitmapSize);\n" \
"	return (newCoord+(scroll/bitmapSize))*multiply;\n" \
"}\n" \
"\n" \
"// main sprite program (with per-pixel lighting)\n" \
"void sprite_pvl(float3 position : POSITION,\n" \
"				float2 texCoord : TEXCOORD0,\n" \
"				out float4 oPosition : POSITION,\n" \
"				out float4 oColor    : COLOR0,\n" \
"				out float2 oTexCoord : TEXCOORD0,\n" \
"				out float2 oVertPos : TEXCOORD1,\n" \
"				uniform float3 topLeft3DPos,\n" \
"				uniform float spaceLength)\n" \
"{\n" \
"	SPRITE_TRANSFORM transform = transformSprite(position);\n" \
"	transform.position.z = (1 - depth) - ((computeVerticalOffsetInPixels(position.y, rectSize.y)) / spaceLength);\n" \
"	oPosition = transform.position;\n" \
"	oVertPos = transform.vertPos;\n" \
"	oTexCoord = transformCoord(texCoord);\n" \
"	\n" \
"	lightColor.a = 1.0;\n" \
"	\n" \
"	float3 vertPos3D = topLeft3DPos + (float3(position.x,0,position.y)*float3(size.x,0,-size.y));\n" \
"	\n" \
"	float3 lightVec = vertPos3D-lightPos;\n" \
"	float squaredDist = dot(lightVec,lightVec);\n" \
"\n" \
"	float3 normalColor = float3(0.0,-1.0,0.0);\n" \
"	float diffuseLight = dot(normalize(lightVec), normalColor);\n" \
"	float squaredRange = lightRange*lightRange;\n" \
"	squaredRange = max(squaredDist, squaredRange);\n" \
"	float attenBias = 1-(squaredDist/squaredRange);\n" \
"	oColor = color0*diffuseLight*attenBias*lightColor*lightIntensity;\n" \
"}\n" \
"\n" \
"\n";

const std::string GLSL_default_ps = 
"precision mediump float;\n" \
"uniform sampler2D diffuse;\n" \
"\n" \
"varying vec4 v_color;\n" \
"varying vec2 v_texCoord;\n" \
"void main()\n" \
"{\n" \
"	gl_FragColor = v_color * texture2D(diffuse, v_texCoord);\n" \
"}\n" \
"\n";

const std::string GLSL_default_vs = 
"attribute vec4 vPosition;\n" \
"attribute vec2 vTexCoord;\n" \
"\n" \
"varying vec4 v_color;\n" \
"varying vec2 v_texCoord;\n" \
"\n" \
"uniform vec2 rectPos;\n" \
"uniform vec2 rectSize;\n" \
"uniform vec2 bitmapSize;\n" \
"\n" \
"uniform mat4 viewMatrix;\n" \
"uniform mat4 rotationMatrix;\n" \
"uniform vec2 screenSize;\n" \
"uniform vec2 size;\n" \
"uniform vec2 entityPos;\n" \
"uniform vec2 center;\n" \
"uniform vec2 cameraPos;\n" \
"\n" \
"uniform vec2 flipMul;\n" \
"uniform vec2 flipAdd;\n" \
"\n" \
"uniform vec4 color0;\n" \
"uniform vec4 color1;\n" \
"uniform vec4 color2;\n" \
"uniform vec4 color3;\n" \
"\n" \
"uniform float depth;\n" \
"\n" \
"vec4 transformSprite(vec3 position)\n" \
"{\n" \
"	vec4 newPos = vec4(position, 1.0);\n" \
"	newPos = newPos * vec4(size, 1.0, 1.0) - vec4(center, 0.0, 0.0);\n" \
"	newPos = (rotationMatrix * newPos);\n" \
"	newPos += vec4(entityPos, 0.0, 0.0)-vec4(screenSize/2.0, 0.0, 0.0)-vec4(cameraPos, 0.0, 0.0);\n" \
"	newPos *= vec4(1.0, -1.0, 1.0, 1.0);\n" \
"	return (viewMatrix * newPos);\n" \
"}\n" \
"\n" \
"vec2 transformCoord(vec2 texCoord)\n" \
"{\n" \
"	vec2 newCoord = texCoord * (rectSize/bitmapSize);\n" \
"	return newCoord + (rectPos/bitmapSize);\n" \
"}\n" \
"\n" \
"vec4 getVertexColor(vec4 position)\n" \
"{\n" \
"	vec4 vertex0 = color0 * (1.0-position.x) * (1.0-position.y);\n" \
"	vec4 vertex1 = color1 * (position.x) * (1.0-position.y);\n" \
"	vec4 vertex2 = color2 * (1.0-position.x) * (position.y);\n" \
"	vec4 vertex3 = color3 * (position.x) * (position.y);\n" \
"	return vertex0 + vertex1 + vertex2 + vertex3;\n" \
"}\n" \
"\n" \
"void main()\n" \
"{\n" \
"	gl_Position = transformSprite(vec3(vPosition.x, vPosition.y, depth)); \n" \
"	v_color = getVertexColor(vPosition);\n" \
"	v_texCoord = transformCoord(vTexCoord);\n" \
"}\n" \
"\n";

const std::string GLSL_hAmbient_vs = 
"#define float4 vec4\n" \
"#define float3 vec3\n" \
"#define float2 vec2\n" \
"\n" \
"attribute vec4 vPosition;\n" \
"attribute vec2 vTexCoord;\n" \
"\n" \
"varying vec4 v_color;\n" \
"varying vec2 v_texCoord;\n" \
"\n" \
"\n" \
"uniform mat4 viewMatrix;\n" \
"uniform mat4 rotationMatrix;\n" \
"\n" \
"uniform vec2 params[17];\n" \
"\n" \
"#define rectPos params[0]\n" \
"#define rectSize params[1]\n" \
"#define center params[2]\n" \
"#define size params[3]\n" \
"#define entityPos params[4]\n" \
"#define cameraPos params[5]\n" \
"#define bitmapSize params[6]\n" \
"#define colorRG params[7]\n" \
"#define colorBA params[8]\n" \
"#define depth params[9]\n" \
"#define screenSize params[10]\n" \
"#define flipAdd params[11]\n" \
"#define flipMul params[12]\n" \
"#define entityPos3DXY params[13]\n" \
"#define entityPos3DZ_parallaxIntensity params[14]\n" \
"#define parallaxOrigin params[15]\n" \
"#define verticalIntensity params[16]\n" \
"\n" \
"\n" \
"\n" \
"\n" \
"float2 computeParallaxOffset(float2 vertPos)\n" \
"{\n" \
"	return ((vertPos - parallaxOrigin) / screenSize.x) * entityPos3DZ_parallaxIntensity.x * entityPos3DZ_parallaxIntensity.y;\n" \
"}\n" \
"\n" \
"float4 transformSprite(float3 position)\n" \
"{\n" \
"	float4 newPos = float4(position, 1.0) * vec4(flipMul, 1.0, 1.0) + vec4(flipAdd, 0.0, 0.0);\n" \
"	newPos = newPos * float4(size, 1.0, 1.0) - float4(center, 0.0, 0.0);\n" \
"	newPos = (rotationMatrix * newPos);\n" \
"	newPos += float4(entityPos, 0.0, 0.0) - float4(cameraPos, 0.0, 0.0);\n" \
"	newPos += float4(computeParallaxOffset(float2(newPos.x,newPos.y)), 0.0, 0.0) - float4(screenSize / 2.0, 0.0, 0.0);\n" \
"	newPos *= float4(1.0,-1.0, 1.0, 1.0);\n" \
"	return (viewMatrix * newPos);\n" \
"}\n" \
"\n" \
"float2 transformCoord(float2 texCoord)\n" \
"{\n" \
"	float2 newCoord = texCoord * (rectSize / bitmapSize);\n" \
"	newCoord += (rectPos / bitmapSize);\n" \
"	return (newCoord);\n" \
"}\n" \
"\n" \
"void main()\n" \
"{\n" \
"	float4 outPos = transformSprite(vec3(vPosition.x, vPosition.y, vPosition.z)); \n" \
"	outPos.z = 1.0 - depth.x;\n" \
"	gl_Position = outPos;\n" \
"	v_color = float4(colorRG, colorBA);\n" \
"	v_texCoord = transformCoord(vTexCoord);\n" \
"}\n" \
"\n" \
"\n";

const std::string GLSL_hPixelLight_vs = 
"#define float4 vec4\n" \
"#define float3 vec3\n" \
"#define float2 vec2\n" \
"\n" \
"attribute vec4 vPosition;\n" \
"attribute vec2 vTexCoord;\n" \
"\n" \
"varying vec4 v_color;\n" \
"varying vec2 v_texCoord;\n" \
"varying vec3 v_vertPos3D;\n" \
"\n" \
"uniform mat4 viewMatrix;\n" \
"uniform mat4 rotationMatrix;\n" \
"\n" \
"uniform vec2 params[17];\n" \
"\n" \
"#define rectPos params[0]\n" \
"#define rectSize params[1]\n" \
"#define center params[2]\n" \
"#define size params[3]\n" \
"#define entityPos params[4]\n" \
"#define cameraPos params[5]\n" \
"#define bitmapSize params[6]\n" \
"#define colorRG params[7]\n" \
"#define colorBA params[8]\n" \
"#define depth params[9]\n" \
"#define screenSize params[10]\n" \
"#define flipAdd params[11]\n" \
"#define flipMul params[12]\n" \
"#define entityPos3DXY params[13]\n" \
"#define entityPos3DZ_parallaxIntensity params[14]\n" \
"#define parallaxOrigin params[15]\n" \
"#define verticalIntensity params[16]\n" \
"\n" \
"uniform float3 topLeft3DPos;\n" \
"\n" \
"float2 computeParallaxOffset(float2 vertPos)\n" \
"{\n" \
"	return ((vertPos - parallaxOrigin) / screenSize.x) * entityPos3DZ_parallaxIntensity.x * entityPos3DZ_parallaxIntensity.y;\n" \
"}\n" \
"\n" \
"float4 transformSprite(float3 position)\n" \
"{\n" \
"	float4 newPos = float4(position, 1.0);\n" \
"	newPos = newPos * float4(size, 1.0, 1.0) - float4(center, 0.0, 0.0);\n" \
"	newPos = (rotationMatrix * newPos);\n" \
"	newPos += float4(entityPos, 0.0, 0.0) - float4(cameraPos, 0.0, 0.0);\n" \
"	newPos += float4(computeParallaxOffset(float2(newPos.x, newPos.y)), 0.0, 0.0) - float4(screenSize / 2.0, 0.0, 0.0);\n" \
"	newPos *= float4(1.0,-1.0, 1.0, 1.0);\n" \
"	return (viewMatrix * newPos);\n" \
"}\n" \
"\n" \
"float2 transformCoord(float2 texCoord)\n" \
"{\n" \
"	float2 newCoord = texCoord * (rectSize / bitmapSize);\n" \
"	newCoord += (rectPos / bitmapSize);\n" \
"	return (newCoord);\n" \
"}\n" \
"\n" \
"void main()\n" \
"{\n" \
"	float4 outPos = transformSprite(vec3(vPosition.x, vPosition.y, vPosition.z)); \n" \
"	outPos.z = 1.0 - depth.x;\n" \
"	gl_Position = outPos;\n" \
"	v_color = float4(colorRG, colorBA);\n" \
"	v_texCoord = transformCoord(vTexCoord);\n" \
"	v_vertPos3D = topLeft3DPos + (vPosition.xyz * float3(size, 1.0));\n" \
"}\n" \
"\n" \
"\n";

const std::string GLSL_hPixelLightDiff_ps = 
"#define float4 vec4\n" \
"#define float3 vec3\n" \
"#define float2 vec2\n" \
"#define float4x4 mat4\n" \
"\n" \
"#ifdef GL_FRAGMENT_PRECISION_HIGH\n" \
"  precision highp float;\n" \
"#else\n" \
"  precision mediump float;\n" \
"#endif\n" \
"\n" \
"uniform sampler2D diffuse;\n" \
"uniform sampler2D normalMap;\n" \
"\n" \
"varying lowp vec4    v_color;\n" \
"varying mediump vec2 v_texCoord;\n" \
"varying vec3         v_vertPos3D;\n" \
"\n" \
"uniform float squaredRange;\n" \
"uniform float3 lightPos;\n" \
"uniform float4 lightColor;\n" \
"\n" \
"void main()\n" \
"{\n" \
"	lowp float4 diffuseColor = texture2D(diffuse, v_texCoord);\n" \
"	lowp float3 normalColor = texture2D(normalMap, v_texCoord).xyz;\n" \
"\n" \
"	float3 lightVec = v_vertPos3D - lightPos;\n" \
"\n" \
"	normalColor = -(2.0 * (normalColor - 0.5));\n" \
"\n" \
"	\n" \
"	lowp float diffuseLight = dot(normalize(lightVec), normalColor);\n" \
"\n" \
"	float squaredDist = dot(lightVec, lightVec);\n" \
"\n" \
"	squaredDist = min(squaredDist, squaredRange);\n" \
"	mediump float attenBias = 1.0 - (squaredDist / squaredRange);\n" \
"\n" \
"	gl_FragColor = (diffuseColor * v_color * lightColor) * attenBias * diffuseLight;\n" \
"}\n" \
"\n" \
"\n";

const std::string GLSL_optimal_vs = 
"attribute vec4 vPosition;\n" \
"attribute vec2 vTexCoord;\n" \
"\n" \
"varying vec4 v_color;\n" \
"varying vec2 v_texCoord;\n" \
"\n" \
"uniform mat4 viewMatrix;\n" \
"uniform mat4 rotationMatrix;\n" \
"\n" \
"uniform vec2 params[13];\n" \
"\n" \
"#define rectPos params[0]\n" \
"#define rectSize params[1]\n" \
"#define center params[2]\n" \
"#define size params[3]\n" \
"#define entityPos params[4]\n" \
"#define cameraPos params[5]\n" \
"#define bitmapSize params[6]\n" \
"#define colorRG params[7]\n" \
"#define colorBA params[8]\n" \
"#define depth params[9]\n" \
"#define screenSize params[10]\n" \
"\n" \
"vec4 transformSprite(vec3 position)\n" \
"{\n" \
"	vec4 newPos = vec4(position, 1.0);\n" \
"	newPos = newPos * vec4(size, 1.0, 1.0) - vec4(center, 0.0, 0.0);\n" \
"	newPos = (rotationMatrix * newPos);\n" \
"	newPos += vec4(entityPos, 0.0, 0.0)-vec4(screenSize/2.0, 0.0, 0.0)-vec4(cameraPos, 0.0, 0.0);\n" \
"	newPos *= vec4(1.0, -1.0, 1.0, 1.0);\n" \
"	return (viewMatrix * newPos);\n" \
"}\n" \
"\n" \
"vec2 transformCoord(vec2 texCoord)\n" \
"{\n" \
"	vec2 newCoord = texCoord * (rectSize/bitmapSize);\n" \
"	return newCoord + (rectPos/bitmapSize);\n" \
"}\n" \
"\n" \
"void main()\n" \
"{\n" \
"	gl_Position = transformSprite(vec3(vPosition.x, vPosition.y, depth.x)); \n" \
"	v_color = vec4(colorRG, colorBA);\n" \
"	v_texCoord = transformCoord(vTexCoord);\n" \
"}\n" \
"\n";

const std::string GLSL_particle_vs = 
"attribute vec4 vPosition;\n" \
"attribute vec2 vTexCoord;\n" \
"\n" \
"varying vec4 v_color;\n" \
"varying vec2 v_texCoord;\n" \
"\n" \
"uniform mat4 viewMatrix;\n" \
"uniform mat4 rotationMatrix;\n" \
"\n" \
"uniform vec2 params[13];\n" \
"\n" \
"#define rectPos params[0]\n" \
"#define rectSize params[1]\n" \
"#define center params[2]\n" \
"#define size params[3]\n" \
"#define entityPos params[4]\n" \
"#define cameraPos params[5]\n" \
"#define bitmapSize params[6]\n" \
"#define colorRG params[7]\n" \
"#define colorBA params[8]\n" \
"#define depth params[9]\n" \
"#define screenSize params[10]\n" \
"\n" \
"vec4 transformSprite(vec3 position)\n" \
"{\n" \
"	vec4 newPos = vec4(position, 1.0);\n" \
"	newPos = newPos * vec4(size, 1.0, 1.0) - vec4(center, 0.0, 0.0);\n" \
"	newPos = (rotationMatrix * newPos);\n" \
"	newPos += vec4(entityPos, 0.0, 0.0)-vec4(screenSize/2.0, 0.0, 0.0)-vec4(cameraPos, 0.0, 0.0);\n" \
"	newPos *= vec4(1.0, -1.0, 1.0, 1.0);\n" \
"	return (viewMatrix * newPos);\n" \
"}\n" \
"\n" \
"vec2 transformCoord(vec2 texCoord)\n" \
"{\n" \
"	vec2 newCoord = texCoord * (rectSize/bitmapSize);\n" \
"	return newCoord + (rectPos/bitmapSize);\n" \
"}\n" \
"\n" \
"void main()\n" \
"{\n" \
"	vec4 pos = transformSprite(vec3(vPosition.x, vPosition.y, vPosition.z));\n" \
"	pos.z = 1.0 - depth.x;\n" \
"	gl_Position = pos; \n" \
"	v_color = vec4(colorRG, colorBA);\n" \
"	v_texCoord = transformCoord(vTexCoord);\n" \
"}\n" \
"\n";

const std::string GLSL_vAmbient_vs = 
"#define float4 vec4\n" \
"#define float3 vec3\n" \
"#define float2 vec2\n" \
"\n" \
"attribute vec4 vPosition;\n" \
"attribute vec2 vTexCoord;\n" \
"\n" \
"varying vec4 v_color;\n" \
"varying vec2 v_texCoord;\n" \
"\n" \
"\n" \
"uniform mat4 viewMatrix;\n" \
"uniform mat4 rotationMatrix;\n" \
"\n" \
"uniform vec2 params[17];\n" \
"\n" \
"#define rectPos params[0]\n" \
"#define rectSize params[1]\n" \
"#define center params[2]\n" \
"#define size params[3]\n" \
"#define entityPos params[4]\n" \
"#define cameraPos params[5]\n" \
"#define bitmapSize params[6]\n" \
"#define colorRG params[7]\n" \
"#define colorBA params[8]\n" \
"#define depth params[9]\n" \
"#define screenSize params[10]\n" \
"#define flipAdd params[11]\n" \
"#define flipMul params[12]\n" \
"#define entityPos3DXY params[13]\n" \
"#define entityPos3DZ_parallaxIntensity params[14]\n" \
"#define parallaxOrigin params[15]\n" \
"#define verticalIntensity params[16]\n" \
"\n" \
"uniform float spaceLength;\n" \
"\n" \
"\n" \
"\n" \
"float computeVerticalOffsetInPixels(float posY, float rectSizeY)\n" \
"{\n" \
"	return ((1.0 - posY) * rectSizeY);\n" \
"}\n" \
"\n" \
"float2 computeParallaxOffset(float2 vertPos, float3 inPosition)\n" \
"{\n" \
"	float vOffset = computeVerticalOffsetInPixels(inPosition.y, rectSize.y);\n" \
"	return ((vertPos - parallaxOrigin) / screenSize.x) * (entityPos3DZ_parallaxIntensity.x + (vOffset * verticalIntensity.x)) * entityPos3DZ_parallaxIntensity.y;\n" \
"}\n" \
"\n" \
"float4 transformSprite(float3 position)\n" \
"{\n" \
"	float4 newPos = float4(position, 1.0) * vec4(flipMul, 1.0, 1.0) + vec4(flipAdd, 0.0, 0.0);\n" \
"	newPos = newPos * float4(size, 1.0, 1.0) - float4(center, 0.0, 0.0);\n" \
"	newPos = (rotationMatrix * newPos);\n" \
"	newPos += float4(entityPos, 0.0, 0.0) - float4(cameraPos, 0.0, 0.0);\n" \
"	newPos += float4(computeParallaxOffset(float2(newPos.x, newPos.y), position), 0.0, 0.0) - float4(screenSize / 2.0, 0.0, 0.0);\n" \
"	newPos *= float4(1.0,-1.0, 1.0, 1.0);\n" \
"	return (viewMatrix * newPos);\n" \
"}\n" \
"\n" \
"float2 transformCoord(float2 texCoord)\n" \
"{\n" \
"	float2 newCoord = texCoord * (rectSize / bitmapSize);\n" \
"	newCoord += (rectPos / bitmapSize);\n" \
"	return (newCoord);\n" \
"}\n" \
"\n" \
"void main()\n" \
"{\n" \
"	float4 outPos = transformSprite(vec3(vPosition.x, vPosition.y, vPosition.z)); \n" \
"	outPos.z = (1.0 - depth.x) - ((computeVerticalOffsetInPixels(vPosition.y, rectSize.y)) / spaceLength);\n" \
"	gl_Position = outPos;\n" \
"	v_color = float4(colorRG, colorBA);\n" \
"	v_texCoord = transformCoord(vTexCoord);\n" \
"}\n" \
"\n" \
"\n";

const std::string GLSL_vPixelLight_vs = 
"#define float4 vec4\n" \
"#define float3 vec3\n" \
"#define float2 vec2\n" \
"\n" \
"attribute vec4 vPosition;\n" \
"attribute vec2 vTexCoord;\n" \
"\n" \
"varying vec4 v_color;\n" \
"varying vec2 v_texCoord;\n" \
"varying vec3 v_vertPos3D;\n" \
"\n" \
"uniform mat4 viewMatrix;\n" \
"uniform mat4 rotationMatrix;\n" \
"\n" \
"uniform vec2 params[17];\n" \
"\n" \
"#define rectPos params[0]\n" \
"#define rectSize params[1]\n" \
"#define center params[2]\n" \
"#define size params[3]\n" \
"#define entityPos params[4]\n" \
"#define cameraPos params[5]\n" \
"#define bitmapSize params[6]\n" \
"#define colorRG params[7]\n" \
"#define colorBA params[8]\n" \
"#define depth params[9]\n" \
"#define screenSize params[10]\n" \
"#define flipAdd params[11]\n" \
"#define flipMul params[12]\n" \
"#define entityPos3DXY params[13]\n" \
"#define entityPos3DZ_parallaxIntensity params[14]\n" \
"#define parallaxOrigin params[15]\n" \
"#define verticalIntensity params[16]\n" \
"\n" \
"uniform float spaceLength;\n" \
"\n" \
"uniform float3 topLeft3DPos;\n" \
"\n" \
"float computeVerticalOffsetInPixels(float posY, float rectSizeY)\n" \
"{\n" \
"	return ((1.0 - posY) * rectSizeY);\n" \
"}\n" \
"\n" \
"float2 computeParallaxOffset(float2 vertPos, float3 inPosition)\n" \
"{\n" \
"	float vOffset = computeVerticalOffsetInPixels(inPosition.y, rectSize.y);\n" \
"	return ((vertPos - parallaxOrigin) / screenSize.x) * (entityPos3DZ_parallaxIntensity.x + (vOffset * verticalIntensity.x)) * entityPos3DZ_parallaxIntensity.y;\n" \
"}\n" \
"\n" \
"float4 transformSprite(float3 position)\n" \
"{\n" \
"	float4 newPos = float4(position, 1.0);\n" \
"	newPos = newPos * float4(size, 1.0, 1.0) - float4(center, 0.0, 0.0);\n" \
"	newPos = (rotationMatrix * newPos);\n" \
"	newPos += float4(entityPos, 0.0, 0.0) - float4(cameraPos, 0.0, 0.0);\n" \
"	newPos += float4(computeParallaxOffset(float2(newPos.x, newPos.y), position), 0.0, 0.0) - float4(screenSize / 2.0, 0.0, 0.0);\n" \
"	newPos *= float4(1.0,-1.0, 1.0, 1.0);\n" \
"	return (viewMatrix * newPos);\n" \
"}\n" \
"\n" \
"float2 transformCoord(float2 texCoord)\n" \
"{\n" \
"	float2 newCoord = texCoord * (rectSize / bitmapSize);\n" \
"	newCoord += (rectPos / bitmapSize);\n" \
"	return (newCoord);\n" \
"}\n" \
"\n" \
"void main()\n" \
"{\n" \
"	float4 outPos = transformSprite(vec3(vPosition.x, vPosition.y, vPosition.z)); \n" \
"	outPos.z = (1.0 - depth.x) - ((computeVerticalOffsetInPixels(vPosition.y, rectSize.y)) / spaceLength);\n" \
"	gl_Position = outPos;\n" \
"	v_color = float4(colorRG, colorBA);\n" \
"	v_texCoord = transformCoord(vTexCoord);\n" \
"	v_vertPos3D = topLeft3DPos + (float3(vPosition.x, 0.0, vPosition.y) * float3(size.x, 0.0,-size.y));\n" \
"}\n" \
"\n" \
"\n";

const std::string GLSL_vPixelLightDiff_ps = 
"#define float4 vec4\n" \
"#define float3 vec3\n" \
"#define float2 vec2\n" \
"#define float4x4 mat4\n" \
"\n" \
"#ifdef GL_FRAGMENT_PRECISION_HIGH\n" \
"  precision highp float;\n" \
"#else\n" \
"  precision mediump float;\n" \
"#endif\n" \
"\n" \
"uniform sampler2D diffuse;\n" \
"uniform sampler2D normalMap;\n" \
"\n" \
"varying lowp vec4    v_color;\n" \
"varying mediump vec2 v_texCoord;\n" \
"varying vec3         v_vertPos3D;\n" \
"\n" \
"uniform float squaredRange;\n" \
"uniform float3 lightPos;\n" \
"uniform float4 lightColor;\n" \
"\n" \
"void main()\n" \
"{\n" \
"	lowp float4 diffuseColor = texture2D(diffuse, v_texCoord);\n" \
"	lowp float3 normalColor = texture2D(normalMap, v_texCoord).xyz;\n" \
"\n" \
"	float3 lightVec = v_vertPos3D - lightPos;\n" \
"\n" \
"	normalColor = -(2.0 * (normalColor - 0.5)) * float3(1.0,-1.0,1.0);\n" \
"	normalColor = normalColor.xzy;\n" \
"\n" \
"	lowp float diffuseLight = dot(normalize(lightVec), normalColor);\n" \
"\n" \
"	float squaredDist = dot(lightVec, lightVec);\n" \
"\n" \
"	squaredDist = min(squaredDist, squaredRange);\n" \
"	mediump float attenBias = 1.0 - (squaredDist / squaredRange);\n" \
"\n" \
"	gl_FragColor = (diffuseColor * v_color * lightColor) * attenBias * diffuseLight;\n" \
"}\n" \
"\n";

const std::string GLSL_default_add1_ps = 
"precision lowp float;\n" \
"uniform sampler2D diffuse;\n" \
"uniform sampler2D t1;\n" \
"\n" \
"varying vec4 v_color;\n" \
"varying vec2 v_texCoord;\n" \
"void main()\n" \
"{\n" \
"	gl_FragColor = v_color * texture2D(diffuse, v_texCoord) + vec4(texture2D(t1, v_texCoord).xyz, 0.0);\n" \
"}\n" \
"\n";

const std::string GLSL_default_default_ps = 
"precision lowp float;\n" \
"uniform sampler2D diffuse;\n" \
"\n" \
"varying vec4 v_color;\n" \
"varying vec2 v_texCoord;\n" \
"void main()\n" \
"{\n" \
"	gl_FragColor = v_color * texture2D(diffuse, v_texCoord);\n" \
"}\n" \
"\n";

const std::string GLSL_default_default_vs = 
"attribute vec4 vPosition;\n" \
"attribute vec2 vTexCoord;\n" \
"\n" \
"varying vec4 v_color;\n" \
"varying vec2 v_texCoord;\n" \
"\n" \
"uniform vec2 rectPos;\n" \
"uniform vec2 rectSize;\n" \
"uniform vec2 bitmapSize;\n" \
"\n" \
"uniform mat4 viewMatrix;\n" \
"uniform mat4 rotationMatrix;\n" \
"uniform vec2 screenSize;\n" \
"uniform vec2 size;\n" \
"uniform vec2 entityPos;\n" \
"uniform vec2 center;\n" \
"uniform vec2 cameraPos;\n" \
"\n" \
"uniform vec2 flipMul;\n" \
"uniform vec2 flipAdd;\n" \
"\n" \
"uniform vec4 color0;\n" \
"uniform vec4 color1;\n" \
"uniform vec4 color2;\n" \
"uniform vec4 color3;\n" \
"\n" \
"uniform float depth;\n" \
"\n" \
"vec4 transformSprite(vec3 position)\n" \
"{\n" \
"	vec4 newPos = vec4(position, 1.0) * vec4(flipMul, 1.0, 1.0) + vec4(flipAdd, 0.0, 0.0);\n" \
"	newPos = newPos * vec4(size, 1.0, 1.0) - vec4(center, 0.0, 0.0);\n" \
"	newPos = (rotationMatrix * newPos);\n" \
"	newPos += vec4(entityPos, 0.0, 0.0)-vec4(screenSize/2.0, 0.0, 0.0)-vec4(cameraPos, 0.0, 0.0);\n" \
"	newPos *= vec4(1.0, -1.0, 1.0, 1.0);\n" \
"	return (viewMatrix * newPos);\n" \
"}\n" \
"\n" \
"vec2 transformCoord(vec2 texCoord)\n" \
"{\n" \
"	vec2 newCoord = texCoord * (rectSize/bitmapSize);\n" \
"	return newCoord + (rectPos/bitmapSize);\n" \
"}\n" \
"\n" \
"vec4 getVertexColor(vec4 position)\n" \
"{\n" \
"	vec4 vertex0 = color0 * (1.0-position.x) * (1.0-position.y);\n" \
"	vec4 vertex1 = color1 * (position.x) * (1.0-position.y);\n" \
"	vec4 vertex2 = color2 * (1.0-position.x) * (position.y);\n" \
"	vec4 vertex3 = color3 * (position.x) * (position.y);\n" \
"	return vertex0 + vertex1 + vertex2 + vertex3;\n" \
"}\n" \
"\n" \
"void main()\n" \
"{\n" \
"	gl_Position = transformSprite(vec3(vPosition.x, vPosition.y, depth)); \n" \
"	v_color = getVertexColor(vPosition);\n" \
"	v_texCoord = transformCoord(vTexCoord);\n" \
"}\n" \
"\n";

const std::string GLSL_default_fastRender_vs = 
"attribute vec4 vPosition;\n" \
"attribute vec2 vTexCoord;\n" \
"\n" \
"varying vec4 v_color;\n" \
"varying vec2 v_texCoord;\n" \
"\n" \
"uniform mat4 viewMatrix;\n" \
"\n" \
"uniform vec2 params[8];\n" \
"\n" \
"#define rectPos params[0]\n" \
"#define rectSize params[1]\n" \
"#define size params[2]\n" \
"#define entityPos params[3]\n" \
"#define bitmapSize params[4]\n" \
"#define screenSize params[5]\n" \
"#define colorRG params[6]\n" \
"#define colorBA params[7]\n" \
"\n" \
"vec4 transformSprite(vec3 position)\n" \
"{\n" \
"	vec4 newPos = vec4(position, 1.0) * vec4(size, 1.0, 1.0);\n" \
"	newPos += vec4(entityPos, 0.0, 0.0)-vec4(screenSize/2.0, 0.0, 0.0);\n" \
"	newPos *= vec4(1.0, -1.0, 1.0, 1.0);\n" \
"	return (viewMatrix * newPos);\n" \
"}\n" \
"\n" \
"vec2 transformCoord(vec2 texCoord)\n" \
"{\n" \
"	vec2 newCoord = texCoord * (rectSize/bitmapSize);\n" \
"	return newCoord + (rectPos/bitmapSize);\n" \
"}\n" \
"\n" \
"void main()\n" \
"{\n" \
"	gl_Position = transformSprite(vec3(vPosition.x, vPosition.y, vPosition.z));\n" \
"	v_color = vec4(colorRG.x, colorRG.y, colorBA.x, colorBA.y);\n" \
"	v_texCoord = transformCoord(vTexCoord);\n" \
"}\n" \
"\n";

const std::string GLSL_default_modulate1_ps = 
"precision lowp float;\n" \
"uniform sampler2D diffuse;\n" \
"uniform sampler2D t1;\n" \
"\n" \
"varying vec4 v_color;\n" \
"varying vec2 v_texCoord;\n" \
"void main()\n" \
"{\n" \
"	gl_FragColor = v_color * texture2D(diffuse, v_texCoord) * texture2D(t1, v_texCoord);\n" \
"}\n" \
"\n";

const std::string GLSL_default_optimal_vs = 
"attribute vec4 vPosition;\n" \
"attribute vec2 vTexCoord;\n" \
"\n" \
"varying vec4 v_color;\n" \
"varying vec2 v_texCoord;\n" \
"\n" \
"uniform mat4 viewMatrix;\n" \
"uniform mat4 rotationMatrix;\n" \
"\n" \
"uniform vec2 params[13];\n" \
"\n" \
"#define rectPos params[0]\n" \
"#define rectSize params[1]\n" \
"#define center params[2]\n" \
"#define size params[3]\n" \
"#define entityPos params[4]\n" \
"#define cameraPos params[5]\n" \
"#define bitmapSize params[6]\n" \
"#define colorRG params[7]\n" \
"#define colorBA params[8]\n" \
"#define depth params[9]\n" \
"#define screenSize params[10]\n" \
"#define flipAdd params[11]\n" \
"#define flipMul params[12]\n" \
"\n" \
"vec4 transformSprite(vec3 position)\n" \
"{\n" \
"	vec4 newPos = vec4(position, 1.0) * vec4(flipMul, 1.0, 1.0) + vec4(flipAdd, 0.0, 0.0);\n" \
"	newPos = newPos * vec4(size, 1.0, 1.0) - vec4(center, 0.0, 0.0);\n" \
"	newPos = (rotationMatrix * newPos);\n" \
"	newPos += vec4(entityPos, 0.0, 0.0)-vec4(screenSize/2.0, 0.0, 0.0)-vec4(cameraPos, 0.0, 0.0);\n" \
"	newPos *= vec4(1.0, -1.0, 1.0, 1.0);\n" \
"	return (viewMatrix * newPos);\n" \
"}\n" \
"\n" \
"vec2 transformCoord(vec2 texCoord)\n" \
"{\n" \
"	vec2 newCoord = texCoord * (rectSize/bitmapSize);\n" \
"	return newCoord + (rectPos/bitmapSize);\n" \
"}\n" \
"\n" \
"void main()\n" \
"{\n" \
"	gl_Position = transformSprite(vec3(vPosition.x, vPosition.y, depth.x)); \n" \
"	v_color = vec4(colorRG, colorBA);\n" \
"	v_texCoord = transformCoord(vTexCoord);\n" \
"}\n" \
"\n";


}

#endif
