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

#include "GLES2UniformParameter.h"
#include "GLES2Shader.h"

namespace gs2d {

int counter = 1;

GLES2UniformParameter::GLES2UniformParameter(const str_type::string& name) :
	m_locations(new LocationMap),
	m_name(name)
{
}

LocationMapPtr GLES2UniformParameter::GetLocations()
{
	return m_locations;
}

const str_type::string& GLES2UniformParameter::GetName() const
{
	return m_name;
}

void GLES2UniformParameter::SetLocations(const LocationMapPtr& locations)
{
	m_locations = locations;
}

inline int GLES2UniformParameter::GetLocation(const GLuint program, const str_type::string& name, const Platform::FileLogger& logger)
{
	LocationMap& locations = *m_locations.get();
	std::map<GLuint, int>::iterator iter = locations.find(program);
	if (iter != locations.end())
	{
		return iter->second;
	}
	else
	{
		const int location = glGetUniformLocation(program, name.c_str());
		GLES2Video::CheckGLError(name + ": uniform parameter not found with glGetUniformLocation", logger);
		str_type::stringstream ss;
		ss << "Location obtained successfully [" << name << "] " << counter++ << ": " << location;
		logger.Log(ss.str(), Platform::FileLogger::INFO);
		locations[program] = location;
		return location;
	}
}

std::map<GLuint, std::map<int, boost::shared_ptr<GLES2UniformParameter> > > GLES2UniformParameter::m_params;
GLenum GLES2UniformParameter::m_activatedTexture = 0xF0000000;
GLenum GLES2UniformParameter::m_boundTexture2D   = 0xF0000000;

void GLES2UniformParameter::AddParam(const GLuint& program, const int& location, const boost::shared_ptr<GLES2UniformParameter>& param)
{
	std::map<GLuint, std::map<int, boost::shared_ptr<GLES2UniformParameter> > >::iterator mapIter = m_params.find(program);
	if (mapIter == m_params.end())
	{
		m_params[program];
		mapIter = m_params.find(program);
	}
	mapIter->second.insert(std::pair<int, boost::shared_ptr<GLES2UniformParameter> >(location, param));

}
	
bool GLES2UniformParameter::IsEqualToAlreadyDefinedParam(const GLuint& program, const int& location,
														 const GLES2UniformParameter* param,
														 const Platform::FileLogger& logger)
{
	GS2D_UNUSED_ARGUMENT(logger);
	std::map<GLuint, std::map<int, boost::shared_ptr<GLES2UniformParameter> > >::iterator mapIter = m_params.find(program);
	if (mapIter != m_params.end())
	{
		std::map<int, boost::shared_ptr<GLES2UniformParameter> >::iterator iter = mapIter->second.find(location);
		if (iter != mapIter->second.end())
		{
			return param->IsEqual((iter->second).get());
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void GLES2UniformParameter::ClearParams()
{
	m_params.clear();
}
	
void GLES2UniformParameter::BindTexture2D(const GLenum& texture)
{
	if (m_boundTexture2D != texture)
	{
		m_boundTexture2D = texture;
		glBindTexture(GL_TEXTURE_2D, texture);
	}
}

void GLES2UniformParameter::ActiveTexture(const GLenum& texture)
{
	if (m_activatedTexture != texture)
	{
		m_activatedTexture = texture;
		glActiveTexture(texture);
	}
}

GLES2UPVec1::GLES2UPVec1(const float v, const str_type::string& name) :
	GLES2UniformParameter(name)
{
	this->v = v;
}
bool GLES2UPVec1::SetParameter(const GLuint program, const str_type::string& name, const Platform::FileLogger& logger)
{
	const int location = GetLocation(program, name, logger);
	//if (!IsEqualToAlreadyDefinedParam(program, location, this, logger))
	{
		//AddParam(program, location, GLES2UniformParameterPtr(new GLES2UPVec1(v, GS_L(""))));
		glUniform1f(location, v);
		//GLES2Video::CheckGLError(name + ": uniform parameter not found with glUniform1f", logger);
	}
	return true;
}

PARAMETER_TYPE GLES2UPVec1::GetType() const
{
	return PT_FLOAT;
}

bool GLES2UPVec1::IsEqual(const gs2d::GLES2UniformParameter *other) const
{
	if (other->GetType() == GetType())
	{
		if (v == static_cast<const GLES2UPVec1*>(other)->v)
		{
			return  true;
		}
	}
	return false;
}

GLES2UPVec2::GLES2UPVec2(const math::Vector2& v, const str_type::string& name) :
	GLES2UniformParameter(name)
{
	this->v = v;
}
bool GLES2UPVec2::SetParameter(const GLuint program, const str_type::string& name, const Platform::FileLogger& logger)
{
	const int location = GetLocation(program, name, logger);
	//if (!IsEqualToAlreadyDefinedParam(program, location, this, logger))
	{
		//AddParam(program, location, GLES2UniformParameterPtr(new GLES2UPVec2(v, GS_L(""))));
		glUniform2fv(location, 1, &v.x);
		//GLES2Video::CheckGLError(name + ": uniform parameter not found with glUniform2fv", logger);
	}
	return true;
}
	
PARAMETER_TYPE GLES2UPVec2::GetType() const
{
	return PT_VECTOR2;
}

bool GLES2UPVec2::IsEqual(const gs2d::GLES2UniformParameter *other) const
{
	if (other->GetType() == GetType())
	{
		if (v == static_cast<const GLES2UPVec2*>(other)->v)
		{
			return true;
		}
	}
	return false;
}

GLES2UPVec3::GLES2UPVec3(const math::Vector3& v, const str_type::string& name) :
	GLES2UniformParameter(name)
{
	this->v = v;
}
bool GLES2UPVec3::SetParameter(const GLuint program, const str_type::string& name, const Platform::FileLogger& logger)
{
	const int location = GetLocation(program, name, logger);
	//if (!IsEqualToAlreadyDefinedParam(program, location, this, logger))
	{
		//AddParam(program, location, GLES2UniformParameterPtr(new GLES2UPVec3(v, GS_L(""))));
		glUniform3fv(location, 1, &v.x);
		//GLES2Video::CheckGLError(name + ": uniform parameter not found with glUniform3fv", logger);
	}
	return true;
}

PARAMETER_TYPE GLES2UPVec3::GetType() const
{
	return PT_VECTOR3;
}

bool GLES2UPVec3::IsEqual(const gs2d::GLES2UniformParameter *other) const
{
	if (other->GetType() == GetType())
	{
		if (v == static_cast<const GLES2UPVec3*>(other)->v)
		{
			return true;
		}
	}
	return false;
}
	
GLES2UPVec4::GLES2UPVec4(const math::Vector4& v, const str_type::string& name) :
	GLES2UniformParameter(name)
{
	this->v = v;
}
bool GLES2UPVec4::SetParameter(const GLuint program, const str_type::string& name, const Platform::FileLogger& logger)
{
	const int location = GetLocation(program, name, logger);
	//if (!IsEqualToAlreadyDefinedParam(program, location, this, logger))
	{
		//AddParam(program, location, GLES2UniformParameterPtr(new GLES2UPVec4(v, GS_L(""))));
		glUniform4fv(location, 1, &v.x);
		//GLES2Video::CheckGLError(name + ": uniform parameter not found with glUniform4fv", logger);
	}
	return true;
}

PARAMETER_TYPE GLES2UPVec4::GetType() const
{
	return PT_VECTOR4;
}

bool GLES2UPVec4::IsEqual(const gs2d::GLES2UniformParameter *other) const
{
	if (other->GetType() == GetType())
	{
		if (v == static_cast<const GLES2UPVec4*>(other)->v)
		{
			return  true;
		}
	}
	return false;
}

GLES2UPMat4x4::GLES2UPMat4x4(const math::Matrix4x4& v, const str_type::string& name) :
	GLES2UniformParameter(name)
{
	this->v = v;
}
bool GLES2UPMat4x4::SetParameter(const GLuint program, const str_type::string& name, const Platform::FileLogger& logger)
{
	const int location = GetLocation(program, name, logger);
	//if (!IsEqualToAlreadyDefinedParam(program, location, this, logger))
	{
		//AddParam(program, location, GLES2UniformParameterPtr(new GLES2UPMat4x4(v, GS_L(""))));
		glUniformMatrix4fv(location, 1, GL_FALSE, (GLfloat*)&v.m[0][0]);
		//GLES2Video::CheckGLError(name + ": uniform parameter not found with glUniformMatrix4fv", logger);
	}
	return true;
}

PARAMETER_TYPE GLES2UPMat4x4::GetType() const
{
	return PT_M4X4;
}

bool GLES2UPMat4x4::IsEqual(const gs2d::GLES2UniformParameter *other) const
{
	if (other->GetType() == GetType())
	{
		if (v == static_cast<const GLES2UPMat4x4*>(other)->v)
		{
			return true;
		}
	}
	return false;
}
	
GLES2UPTexture::GLES2UPTexture(const GLenum texturePass, GLuint texture, const GLint unit, const str_type::string& name) :
	GLES2UniformParameter(name)
{
	this->texturePass = texturePass;
	this->texture = texture;
	this->unit = unit;
}
bool GLES2UPTexture::SetParameter(const GLuint program, const str_type::string& name, const Platform::FileLogger& logger)
{
	ActiveTexture(texturePass);
	BindTexture2D(texture);
	const int location = GetLocation(program, name, logger);
	//if (!IsEqualToAlreadyDefinedParam(program, location, this, logger))
	{
		//AddParam(program, location, GLES2UniformParameterPtr(new GLES2UPTexture(texturePass, texture, unit, GS_L(""))));
		glUniform1i(location, unit);
		//GLES2Video::CheckGLError(name + ": uniform parameter not found with glUniform1i", logger);
	}
	return true;
}

PARAMETER_TYPE GLES2UPTexture::GetType() const
{
	return PT_TEXTURE;
}

bool GLES2UPTexture::IsEqual(const gs2d::GLES2UniformParameter *other) const
{
	if (other->GetType() == GetType())
	{
		const bool unitChanged = (unit != static_cast<const GLES2UPTexture*>(other)->unit);
		return !unitChanged;
	}
	else
	{
		return false;
	}
}
	
GLES2UPVec2Array::GLES2UPVec2Array(const boost::shared_array<const gs2d::math::Vector2>& v, const unsigned int count, const str_type::string& name) :
	GLES2UniformParameter(name),
	count(static_cast<GLsizei>(count))
{
	this->va = v;
}
bool GLES2UPVec2Array::SetParameter(const GLuint program, const str_type::string& name, const Platform::FileLogger& logger)
{
	const int location = GetLocation(program, name, logger);
	//if (!IsEqualToAlreadyDefinedParam(program, location, this, logger))
	{
		//AddParam(program, location, GLES2UniformParameterPtr(new GLES2UPVec2Array(va, count, GS_L(""))));
		glUniform2fv(location, count, &(va.get()->x));
		//GLES2Video::CheckGLError(name + ": uniform parameter not found with glUniform2fv", logger);
	}
	return true;
}

PARAMETER_TYPE GLES2UPVec2Array::GetType() const
{
	return PT_VECTOR2_ARRAY;
}

bool GLES2UPVec2Array::IsEqual(const gs2d::GLES2UniformParameter *other) const
{
	if (other->GetType() == GetType())
	{
		for (GLsizei t = 0; t < count; t++)
		{
			if (va[t] != static_cast<const GLES2UPVec2Array*>(other)->va[t])
			{
				return false;
			}
		}
	}
	return true;
}

} // namespace gs2d
