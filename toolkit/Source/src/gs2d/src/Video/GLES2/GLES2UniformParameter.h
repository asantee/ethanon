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

#ifndef GLES2_UNIFORM_PARAMETER_H_
#define GLES2_UNIFORM_PARAMETER_H_

#include "GLES2Video.h"

#ifdef APPLE_IOS
  #include <OpenGLES/ES2/gl.h>
  #include <OpenGLES/ES2/glext.h>
#endif

#ifdef ANDROID
  #include <GLES2/gl2.h>
  #include <GLES2/gl2ext.h>
#endif

#include "../../../../tsl/hopscotch_map.h"

namespace gs2d {

typedef tsl::hopscotch_map<GLuint, int> LocationMap;
typedef boost::shared_ptr<LocationMap> LocationMapPtr;

enum PARAMETER_TYPE
{
	PT_FLOAT = 0,
	PT_VECTOR2 = 1,
	PT_VECTOR3,
	PT_VECTOR4,
	PT_VECTOR2_ARRAY,
	PT_M4X4,
	PT_TEXTURE
};
	
class GLES2UniformParameter
{
	friend class GLES2ShaderContext;
	friend class GLES2Texture;
	LocationMapPtr m_locations;
	str_type::string m_parameterName;
	str_type::string m_shaderName;
	static GLenum m_activatedTexture;
	static GLenum m_boundTexture2D;

protected:
	static void ClearParams();
	static tsl::hopscotch_map<GLuint, tsl::hopscotch_map<int, boost::shared_ptr<GLES2UniformParameter> > > m_params;
	static bool IsEqualToAlreadyDefinedParam(const GLuint& program, const int& location, const GLES2UniformParameter* param, const Platform::FileLogger& logger);
	static void AddParam(const GLuint& program, const int& location, const boost::shared_ptr<GLES2UniformParameter>& param);
	static void BindTexture2D(const GLenum& texture);
	static void ActiveTexture(const GLenum& texture);
public:
	const str_type::string& GetParameterName() const;
	GLES2UniformParameter(const str_type::string& shaderName, const str_type::string& parameterName);
	void SetLocations(const LocationMapPtr& locations);
	LocationMapPtr GetLocations();
	inline int GetLocation(const GLuint program, const Platform::FileLogger& logger);
	virtual bool SetParameter(const GLuint program, const Platform::FileLogger& logger) = 0;
	virtual bool IsEqual(const GLES2UniformParameter* other) const = 0;
	virtual PARAMETER_TYPE GetType() const = 0;
};

class GLES2UPVec1 : public GLES2UniformParameter
{
	GLfloat v;
public:
	GLES2UPVec1(const float v, const str_type::string& shaderName, const str_type::string& parameterName);
	bool SetParameter(const GLuint program, const Platform::FileLogger& logger);
	PARAMETER_TYPE GetType() const;
	bool IsEqual(const GLES2UniformParameter* other) const;
};

class GLES2UPVec2 : public GLES2UniformParameter
{
	math::Vector2 v;
public:
	GLES2UPVec2(const math::Vector2& v, const str_type::string& shaderName, const str_type::string& parameterName);
	bool SetParameter(const GLuint program, const Platform::FileLogger& logger);
	PARAMETER_TYPE GetType() const;
	bool IsEqual(const GLES2UniformParameter* other) const;
};

class GLES2UPVec3 : public GLES2UniformParameter
{
	math::Vector3 v;
public:
	GLES2UPVec3(const math::Vector3& v, const str_type::string& shaderName, const str_type::string& parameterName);
	bool SetParameter(const GLuint program, const Platform::FileLogger& logger);
	PARAMETER_TYPE GetType() const;
	bool IsEqual(const GLES2UniformParameter* other) const;
};

class GLES2UPVec4 : public GLES2UniformParameter
{
	math::Vector4 v;
public:
	GLES2UPVec4(const math::Vector4& v, const str_type::string& shaderName, const str_type::string& parameterName);
	bool SetParameter(const GLuint program, const Platform::FileLogger& logger);
	PARAMETER_TYPE GetType() const;
	bool IsEqual(const GLES2UniformParameter* other) const;
};

class GLES2UPMat4x4 : public GLES2UniformParameter
{
	math::Matrix4x4 v;
public:
	GLES2UPMat4x4(const math::Matrix4x4& v, const str_type::string& shaderName, const str_type::string& parameterName);
	bool SetParameter(const GLuint program, const Platform::FileLogger& logger);
	PARAMETER_TYPE GetType() const;
	bool IsEqual(const GLES2UniformParameter* other) const;
};

class GLES2UPTexture : public GLES2UniformParameter
{
	GLenum texturePass;
	GLuint texture;
	GLint unit;
public:
	GLES2UPTexture(const GLenum texturePass, GLuint texture, const GLint unit, const str_type::string& shaderName, const str_type::string& parameterName);
	bool SetParameter(const GLuint program, const Platform::FileLogger& logger);
	PARAMETER_TYPE GetType() const;
	bool IsEqual(const GLES2UniformParameter* other) const;
};

class GLES2UPVec2Array : public GLES2UniformParameter
{
	boost::shared_array<const math::Vector2> va;
	GLsizei count;
public:
	GLES2UPVec2Array(const boost::shared_array<const math::Vector2>& v, const unsigned int count, const str_type::string& shaderName, const str_type::string& parameterName);
	bool SetParameter(const GLuint program, const Platform::FileLogger& logger);
	PARAMETER_TYPE GetType() const;
	bool IsEqual(const GLES2UniformParameter* other) const;
};

typedef boost::shared_ptr<GLES2UniformParameter> GLES2UniformParameterPtr;
typedef boost::shared_ptr<GLES2UPVec1> GLES2UPVec1Ptr;
typedef boost::shared_ptr<GLES2UPVec2> GLES2UPVec2Ptr;
typedef boost::shared_ptr<GLES2UPVec3> GLES2UPVec3Ptr;
typedef boost::shared_ptr<GLES2UPVec4> GLES2UPVec4Ptr;
typedef boost::shared_ptr<GLES2UPMat4x4> GLES2UPMat4x4Ptr;
typedef boost::shared_ptr<GLES2UPTexture> GLES2UPTexturePtr;
typedef boost::shared_ptr<GLES2UPVec2Array> GLES2UPVec2ArrayPtr;

} // namespace gs2d

#endif
