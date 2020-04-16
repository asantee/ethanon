#include "Shader.h"

namespace gs2d {

Shader::TextureShaderParameter::TextureShaderParameter(const TexturePtr& texture, const unsigned int index) :
	m_texture(texture),
	m_index(index)
{
}

void Shader::TextureShaderParameter::SetConstant(const std::string& name, const ShaderPtr& shader)
{
	shader->SetTexture(name, m_texture, m_index);
}

Shader::FloatShaderParameter::FloatShaderParameter(const float f) :
	m_f(f)
{
}

void Shader::FloatShaderParameter::SetConstant(const std::string& name, const ShaderPtr& shader)
{
	shader->SetConstant(name, (float)m_f);
}

Shader::Vector2ShaderParameter::Vector2ShaderParameter(const math::Vector2& v) :
	m_v(v)
{
}

void Shader::Vector2ShaderParameter::SetConstant(const std::string& name, const ShaderPtr& shader)
{
	shader->SetConstant(name, m_v);
}

Shader::Vector3ShaderParameter::Vector3ShaderParameter(const math::Vector3& v) :
	m_v(v)
{
}

void Shader::Vector3ShaderParameter::SetConstant(const std::string& name, const ShaderPtr& shader)
{
	shader->SetConstant(name, m_v);
}

Shader::Vector4ShaderParameter::Vector4ShaderParameter(const math::Vector4& v) :
	m_v(v)
{
}

void Shader::Vector4ShaderParameter::SetConstant(const std::string& name, const ShaderPtr& shader)
{
	shader->SetConstant(name, m_v);
}

} // namespace gs2d
