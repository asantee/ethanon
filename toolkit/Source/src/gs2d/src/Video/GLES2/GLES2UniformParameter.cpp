#include "GLES2UniformParameter.h"
#include "GLES2Shader.h"

namespace gs2d {

int counter = 1;

GLES2UniformParameter::GLES2UniformParameter(const str_type::string& shaderName, const str_type::string& parameterName) :
	m_locations(new LocationMap),
	m_parameterName(parameterName),
	m_shaderName(shaderName)
{
}

LocationMapPtr GLES2UniformParameter::GetLocations()
{
	return m_locations;
}

const str_type::string& GLES2UniformParameter::GetParameterName() const
{
	return m_parameterName;
}

void GLES2UniformParameter::SetLocations(const LocationMapPtr& locations)
{
	m_locations = locations;
}

inline int GLES2UniformParameter::GetLocation(const GLuint program, const Platform::FileLogger& logger)
{
	LocationMap& locations = *m_locations.get();
	tsl::hopscotch_map<GLuint, int>::iterator iter = locations.find(program);
	if (iter != locations.end())
	{
		return iter->second;
	}
	else
	{
		str_type::stringstream ss;
		const str_type::string& name = GetParameterName();

		glUseProgram(program);
		const int location = glGetUniformLocation(program, name.c_str());
		if (!GLES2Video::CheckGLError(name + ": uniform parameter not found with glGetUniformLocation", logger))
		{
			ss << "Location obtained successfully [" << name << "] " << counter++ << ": " << location;
			logger.Log(ss.str(), Platform::FileLogger::INFO);
			locations[program] = location;
			return location;
		}
		else
		{
			GLuint currentProgram;
			glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&currentProgram);
			ss << "Couldn't get location for parameter " << name << " on shader " << m_shaderName
			   << " (current: " << currentProgram << " / " << "should be " << program << ")";
			logger.Log(ss.str(), Platform::FileLogger::ERROR);
			return -1;
		}
	}
}

tsl::hopscotch_map<GLuint, tsl::hopscotch_map<int, boost::shared_ptr<GLES2UniformParameter> > > GLES2UniformParameter::m_params;
GLenum GLES2UniformParameter::m_activatedTexture = 0xF0000000;
GLenum GLES2UniformParameter::m_boundTexture2D   = 0xF0000000;

void GLES2UniformParameter::AddParam(const GLuint& program, const int& location, const boost::shared_ptr<GLES2UniformParameter>& param)
{
	tsl::hopscotch_map<GLuint, tsl::hopscotch_map<int, boost::shared_ptr<GLES2UniformParameter> > >::iterator mapIter = m_params.find(program);
	if (mapIter == m_params.end())
	{
		m_params[program];
		mapIter = m_params.find(program);
	}
	mapIter.value().insert(std::pair<int, boost::shared_ptr<GLES2UniformParameter> >(location, param));

}
	
bool GLES2UniformParameter::IsEqualToAlreadyDefinedParam(const GLuint& program, const int& location,
														 const GLES2UniformParameter* param,
														 const Platform::FileLogger& logger)
{
	GS2D_UNUSED_ARGUMENT(logger);
	tsl::hopscotch_map<GLuint, tsl::hopscotch_map<int, boost::shared_ptr<GLES2UniformParameter> > >::iterator mapIter = m_params.find(program);
	if (mapIter != m_params.end())
	{
		tsl::hopscotch_map<int, boost::shared_ptr<GLES2UniformParameter> >::iterator iter = mapIter.value().find(location);
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

GLES2UPVec1::GLES2UPVec1(const float v, const str_type::string& shaderName, const str_type::string& parameterName) :
	GLES2UniformParameter(shaderName, parameterName)
{
	this->v = v;
}
bool GLES2UPVec1::SetParameter(const GLuint program, const Platform::FileLogger& logger)
{
	const int location = GetLocation(program, logger);
	glUniform1f(location, v);
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

GLES2UPVec2::GLES2UPVec2(const math::Vector2& v, const str_type::string& shaderName, const str_type::string& parameterName) :
	GLES2UniformParameter(shaderName, parameterName)
{
	this->v = v;
}
bool GLES2UPVec2::SetParameter(const GLuint program, const Platform::FileLogger& logger)
{
	const int location = GetLocation(program, logger);
	glUniform2fv(location, 1, &v.x);
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

GLES2UPVec3::GLES2UPVec3(const math::Vector3& v, const str_type::string& shaderName, const str_type::string& parameterName) :
	GLES2UniformParameter(shaderName, parameterName)
{
	this->v = v;
}
bool GLES2UPVec3::SetParameter(const GLuint program, const Platform::FileLogger& logger)
{
	const int location = GetLocation(program, logger);
	glUniform3fv(location, 1, &v.x);
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
	
GLES2UPVec4::GLES2UPVec4(const math::Vector4& v, const str_type::string& shaderName, const str_type::string& parameterName) :
	GLES2UniformParameter(shaderName, parameterName)
{
	this->v = v;
}
bool GLES2UPVec4::SetParameter(const GLuint program, const Platform::FileLogger& logger)
{
	const int location = GetLocation(program, logger);
	glUniform4fv(location, 1, &v.x);
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

GLES2UPMat4x4::GLES2UPMat4x4(const math::Matrix4x4& v, const str_type::string& shaderName, const str_type::string& parameterName) :
	GLES2UniformParameter(shaderName, parameterName)
{
	this->v = v;
}
bool GLES2UPMat4x4::SetParameter(const GLuint program, const Platform::FileLogger& logger)
{
	const int location = GetLocation(program, logger);
	glUniformMatrix4fv(location, 1, GL_FALSE, (GLfloat*)&v.m[0][0]);
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
	
GLES2UPTexture::GLES2UPTexture(
	const GLenum texturePass,
	GLuint texture,
	const GLint unit,
	const str_type::string& shaderName,
	const str_type::string& parameterName) :
	GLES2UniformParameter(shaderName, parameterName)
{
	this->texturePass = texturePass;
	this->texture = texture;
	this->unit = unit;
}
bool GLES2UPTexture::SetParameter(const GLuint program, const Platform::FileLogger& logger)
{
	ActiveTexture(texturePass);
	BindTexture2D(texture);
	const int location = GetLocation(program, logger);
	glUniform1i(location, unit);
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
	
GLES2UPVec2Array::GLES2UPVec2Array(
	const boost::shared_array<const gs2d::math::Vector2>& v,
	const unsigned int count,
	const str_type::string& shaderName,
	const str_type::string& parameterName) :
	GLES2UniformParameter(shaderName, parameterName),
	count(static_cast<GLsizei>(count))
{
	this->va = v;
}

bool GLES2UPVec2Array::SetParameter(const GLuint program, const Platform::FileLogger& logger)
{
	const int location = GetLocation(program, logger);
	glUniform2fv(location, count, &(va.get()->x));
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
