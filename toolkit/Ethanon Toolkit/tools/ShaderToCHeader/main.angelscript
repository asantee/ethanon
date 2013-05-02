void main()
{
	LoadScene("empty");

	convertShaderCodeToCHeader();
	Exit();
}

void convertShaderCodeToCHeader()
{
	const string[] shaders = {
		"Cg/defaultVS.cg",
		"Cg/dynaShadowVS.cg",
		"Cg/hAmbientVS.cg",
		"Cg/hPixelLightDiff.cg",
		"Cg/hPixelLightSpec.cg",
		"Cg/hPixelLightVS.cg",
		"Cg/hVertexLightShader.cg",
		"Cg/particleVS.cg",
		"Cg/vAmbientVS.cg",
		"Cg/vPixelLightDiff.cg",
		"Cg/vPixelLightSpec.cg",
		"Cg/vPixelLightVS.cg",
		"Cg/vVertexLightShader.cg",
		"GLSL/default.ps",
		"GLSL/default.vs",
		"GLSL/hAmbient.vs",
		"GLSL/hPixelLight.vs",
		"GLSL/hPixelLightDiff.ps",
		"GLSL/optimal.vs",
		"GLSL/particle.vs",
		"GLSL/vAmbient.vs",
		"GLSL/vPixelLight.vs",
		"GLSL/vPixelLightDiff.ps",
		"GLSL/default/add1.ps",
		"GLSL/default/default.ps",
		"GLSL/default/default.vs",
		"GLSL/default/fastRender.vs",
		"GLSL/default/modulate1.ps",
		"GLSL/default/optimal.vs"
	};

	const string relativePath = GetResourceDirectory() + "../../../Source/src/shaders/";

	string output = filePrefix;

	for (uint t = 0; t < shaders.length(); t++)
	{
		const string filePath = relativePath + shaders[t];
		if (FileExists(filePath))
		{
			output += "const std::string " + extractStringDeclName(shaders[t]) + " = \n";

			const string shaderCode = GetStringFromFile(filePath);
			const string[] lines = split(shaderCode, "\n");
			for (uint l = 0; l < lines.length(); l++)
			{
				output += "\"" + lines[l] + "\\n" + "\" " + "\\" + "\n";
			}
			output += "\"\\n\";\n\n";
		}
	}

	output += fileSufix;
	SaveStringToFile(relativePath + "shaders.h", output);
}

const string filePrefix =
"/* This file has been generated with the ShaderToCHeader tool. Do not edit! */\n\n"
"\n"
"#ifndef ETH_DEFAULT_SHADERS_H_\n"
"#define ETH_DEFAULT_SHADERS_H_\n"
"\n"
"#include <string>\n"
"\n"
"namespace ETHGlobal {\n"
"\n";

const string fileSufix = "\n}\n\n#endif\n";

string extractStringDeclName(string fileName)
{
	string r;
	string[] majorPieces = split(fileName, ".");
	fileName = majorPieces[0];

	fileName = replace(fileName, "\\", "/");

	string[] pieces = split(fileName, "/");
	for (uint t = 0; t < pieces.length(); t++)
	{
		if (t != 0)
			r += "_";
		r += pieces[t];
	}
	r += "_" + majorPieces[1];
	return r;
}

string replace(const string &in str, const string &in sequence, const string &in replace)
{
	string r;
	string[] pieces = split(str, sequence);
	const uint len = pieces.length();
	if (len >= 2)
	{
		for (uint t = 0; t < pieces.length() - 1; t++)
		{
			r += pieces[t] + replace;
		}
		r += pieces[len - 1];
	}
	else
	{
		r = str;
	}
	return r;
}

string[] split(string str, const string c)
{
	string[] v;
	uint pos;
	while ((pos = str.find(c)) != NPOS)
	{
		v.insertLast(str.substr(0, pos));
		str = str.substr(pos + c.length(), NPOS);
	}
	v.insertLast(str);
	return v;
}
