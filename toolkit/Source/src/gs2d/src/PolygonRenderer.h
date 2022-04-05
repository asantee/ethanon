#ifndef GS2D_VIDEO_POLYGON_RENDERER_H_
#define GS2D_VIDEO_POLYGON_RENDERER_H_

#include "Shader.h"

#include "Math/Vector3.h"

#include <boost/shared_ptr.hpp>

#include <vector>

namespace gs2d {

class PolygonRenderer;

typedef boost::shared_ptr<PolygonRenderer> PolygonRendererPtr;

class PolygonRenderer
{
public:
	enum POLYGON_MODE
	{
		TRIANGLE_LIST,
		TRIANGLE_STRIP,
		TRIANGLE_FAN
	};

	struct Vertex
	{
		Vertex(const math::Vector3& p, const math::Vector3& n, const math::Vector2& t);
		math::Vector3 pos;
		math::Vector2 texCoord;
		math::Vector3 normal;
	};

	virtual void BeginRendering(const ShaderPtr& shader) = 0;
	virtual void Render() = 0;
	virtual void EndRendering() = 0;
};

} // namespace gs2d

#endif
