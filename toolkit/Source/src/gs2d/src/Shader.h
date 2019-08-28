#ifndef GS2D_SHADER_H_
#define GS2D_SHADER_H_

#include "Math/Matrix4x4.h"
#include "Math/Color.h"
#include "Texture.h"

#include <hopscotch_map.h>

#include <boost/shared_ptr.hpp>

namespace gs2d {

class ShaderContext
{
public:
};

typedef boost::shared_ptr<ShaderContext> ShaderContextPtr;

class Video;
class Shader;

typedef std::shared_ptr<Shader> ShaderPtr;

class Shader
{
public:
    class ShaderParameter
    {
    public:
        virtual void SetConstant(const std::string& name, const ShaderPtr& shader) = 0;
    };
    
    class TextureShaderParameter : public ShaderParameter
    {
        TexturePtr m_texture;
        unsigned int m_index;
    public:
        TextureShaderParameter(const TexturePtr& texture, const unsigned int index);
        void SetConstant(const std::string& name, const ShaderPtr& shader);
    };

    class FloatShaderParameter : public ShaderParameter
    {
        float m_f;
    public:
        FloatShaderParameter(const float f);
        void SetConstant(const std::string& name, const ShaderPtr& shader);
    };

    class Vector2ShaderParameter : public ShaderParameter
    {
        math::Vector2 m_v;
    public:
        Vector2ShaderParameter(const math::Vector2& v);
        void SetConstant(const std::string& name, const ShaderPtr& shader);
    };

    class Vector3ShaderParameter : public ShaderParameter
    {
        math::Vector3 m_v;
    public:
        Vector3ShaderParameter(const math::Vector3& v);
        void SetConstant(const std::string& name, const ShaderPtr& shader);
    };

    class Vector4ShaderParameter : public ShaderParameter
    {
        math::Vector4 m_v;
    public:
        Vector4ShaderParameter(const math::Vector4& v);
        void SetConstant(const std::string& name, const ShaderPtr& shader);
    };

	virtual bool LoadShaderFromFile(
		ShaderContextPtr context,
        const std::string& vsFileName,
        const std::string& vsEntry,
        const std::string& psFileName,
        const std::string& psEntry) = 0;

	virtual bool LoadShaderFromString(
		ShaderContextPtr context,
        const std::string& vsShaderName,
        const std::string& vsCodeAsciiString,
        const std::string& vsEntry,
        const std::string& psShaderName,
        const std::string& psCodeAsciiString,
        const std::string& psEntry) = 0;

	virtual void SetConstant(const std::string& name, const math::Vector4 &v) = 0;
	virtual void SetConstant(const std::string& name, const math::Vector3 &v) = 0;
	virtual void SetConstant(const std::string& name, const math::Vector2 &v) = 0;
	virtual void SetConstant(const std::string& name, const float x) = 0;
	virtual void SetConstant(const std::string& name, const int n) = 0;
	virtual void SetConstantArray(const std::string& name, unsigned int nElements, const math::Vector2* v) = 0;
    virtual void SetConstantArray(const std::string& name, unsigned int nElements, const math::Vector4* v) = 0;
	virtual void SetMatrixConstant(const std::string& name, const math::Matrix4x4 &matrix) = 0;
	virtual void SetTexture(const std::string& name, TexturePtr pTexture, const unsigned int index) = 0;
	
	virtual void SetShader() = 0;
};

typedef boost::shared_ptr<Shader::ShaderParameter> ShaderParameterPtr;
typedef tsl::hopscotch_map<std::string, ShaderParameterPtr> ShaderParameters;
typedef boost::shared_ptr<ShaderParameters> ShaderParametersPtr;

} // namespace gs2d

#endif
